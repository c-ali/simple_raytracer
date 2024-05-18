#include <iostream>
#include <thread>
#include <future>
#include <functional>
#include <list>
#include "view.h"
#include "algebra.h"
#include "shading.h"

view::view(int width, int height, vec3f viewer_pos, vec3f viewing_dir, mesh &msh, float viewing_dst, std::vector<vec3f> light_srcs, std::vector<float> light_intensites) :
    img_width(width), img_height(height), viewer_pos(viewer_pos), w(viewing_dir.normalized()), msh(msh), viewing_dst(viewing_dst), light_srcs(light_srcs), light_intensites(light_intensites){
    //initialize random seed
    srand (static_cast <unsigned> (time(0)));
    //initialize shader
    shdr = std::make_shared<phong_shader>(light_srcs, viewer_pos, light_intensites);
}

float randf(float scale){
    //returns random float between 0 and scale
    return scale * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

QImage view::render(){
    int hit_count = 0;
    int thread_len = (int) img_width / num_threads;
    std::vector<std::future<void>> futures;
    img = QImage(img_width, img_height, QImage::Format_RGB32);
    //loop over pixels
    for(int j = 0; j < num_threads; ++j){
        int start = j*thread_len;
        int end = (j < num_threads - 1) ? (j+1)*thread_len : img_width;
        if(num_threads > 1)
                futures.push_back(std::move(std::async(std::launch::async, &view::compute_lines, std::ref(*this), start, end)));
        else
            compute_lines(start, end);
    }

    //wait for all threads
    for(size_t j = 0; j < futures.size(); ++j){
        futures[j].wait();
    }
    return img;
}

void view::compute_lines(int j_start, int j_end){
    for(int j = j_start; j < j_end; ++j){
        //computes line j_start to j_end
        QRgb *pix = (QRgb*) img.scanLine(j) ;
        for(int i = 0; i < img_height; ++i){
            float u_offset, v_offset;
            vec3f ray_direction, ray_origin;
            vec3f sum(0,0,0);
            for(int k = 0; k < samples_per_ray; k++){
                //compute pixel coordinates. jitter if necessary
                if(anti_alias){
                    //random number between 0 and 1
                    float rx = randf(1);
                    float ry = randf(1);

                    u_offset = -1 + 2 * (i + 0.5 + rx) / img_width;
                    v_offset = -1 + 2 * (j + 0.5 + ry) / img_height;
                }
                else{
                    u_offset = -1 + 2 * (i + 0.5) / img_width;
                    v_offset = -1 + 2 * (j + 0.5) / img_height;
                }


                //compute ray direction and origin for different projection modes
                switch(MODE){
                    case 'o': {
                        ray_direction = -w;
                        ray_origin = viewer_pos + u * u_offset + v * v_offset;
                    } break;
                    case 'p': {
                        ray_direction = -viewing_dst * w + u * u_offset + v * v_offset;
                        ray_origin = viewer_pos;
                    } break;
                }

                if(focal_dist > 0){
                    float rx_dof = randf(aperture); //offset for DOF
                    float ry_dof = randf(aperture); //offset for DOF
                    vec3f focal_pt = ray_origin + ray_direction * focal_dist;
                    ray_origin = ray_origin + u * rx_dof + v * ry_dof;
                    ray_direction = focal_pt - ray_origin;
                }


                ray light_ray = ray(ray_origin, ray_direction);

                if(!path_tracing)
                    sum = sum + ray_color(light_ray, eps, max_dist, 0);
                else
                    sum = sum + trace_color(light_ray, 0);
            }
            sum = sum / samples_per_ray;

            //adjust numeric range for path tracing
            if(path_tracing)
                sum = sum * lighting_fac;

            sum = sum.bounded();
            //go to correct pixel and write with scanline
            pix++;
            *pix = qRgb(sum[0], sum[1], sum[2]);
        }
    }
}

vec3f view::ray_color(ray r, float t0, float t1, int recursion_depth){
    //use regular raytracing to determine the color of a pixel

    vec3f color = background_color;

    hit_record hr = hit_record(); //view hit record
    hit_record sr = hit_record(); //shadow record

    //check if ray hits any object
    if(msh.hit(r, t0, t1, hr)){
        vec3f const sect_pt = *hr.get_sect_coords();
        if(shadows){
            std::vector<bool> in_shadow;
            //check for shadow by each light source
            for(size_t k = 0; k < light_srcs.size(); ++k){
                vec3f ray_dir = light_srcs[k] - sect_pt;
                ray shadow_ray = ray(sect_pt, ray_dir);
                in_shadow.push_back(msh.hit(shadow_ray, t0, t1, sr));
            }
            color = shdr->shade(hr, in_shadow);
        }
        else
            color = shdr->shade(hr, std::vector<bool>(light_srcs.size(),false));


        //if specular reflection is used, recurse call
        if(hr.is_specular() && recursion_depth < max_recursion_depth){
            ray reflected_ray = get_reflected_ray(r, sect_pt, *hr.get_normal());
            return ray_color(reflected_ray, eps, max_dist, recursion_depth+1);
        }
    }

    return color;

}


vec3f view::trace_color(ray r, int recursion_depth){
    //use path_tracing to determine the color of a pixel
    //supports only diffuse surfaces for now

    vec3f color = background_color;
    hit_record hr = hit_record(); //view hit record

    //check if ray hits any object
    if(msh.hit(r, eps, max_dist, hr)){

        vec3f surf_col(*hr.get_surface_color());
        vec3f sect_pt(*hr.get_sect_coords());
        color = *hr.get_emittence();

        //play russian roulette
        float termination_prob = std::max(surf_col.x, std::max(surf_col.y, surf_col.z)) * roulette_prob;
        if(randf(1) > termination_prob)
            //calcel ray
            return color;

        //cast new ray in hemisphere with correct probability
        vec3f normal = *hr.get_normal();
        ray r_new = (cos_weighted) ? random_ray_in_hemisphere_cosw(sect_pt, normal) : random_ray_in_hemisphere_reject(sect_pt, normal);

        //compute brdf;
        vec3f brdf = surf_col / M_PI;
        vec3f indirect_light(0,0,0);


        //if not emitting trace path
        if(color*vec3f(1,1,1) == 0){
            float reciever_angle = r_new.dir * normal;
            float inv_pdf = (cos_weighted) ? 2 : reciever_angle * inv_p_diffuse;
            //recursively trace reflected light and apply rendering eq
            indirect_light = (pt_mult(brdf, trace_color(r_new, recursion_depth + 1)) *  inv_pdf);

            //next event estimation
            vec3f direct_light(0,0,0);
            if(next_event){
                for(size_t i = 0; i < msh.emitting_faces.size(); ++i){
                    //send shadow ray
                    hit_record sr = hit_record();
                    ray shadow_ray = ray(sect_pt,  msh.emitting_faces[i]->random_surface_pt() - sect_pt);
                    if(msh.emitting_faces[i]->hit(shadow_ray, eps, max_dist, sr)){
                        float emitter_angle = -shadow_ray.dir.normalized() * *sr.get_normal();
                        float dist = shadow_ray.dir*shadow_ray.dir;
                        direct_light = pt_mult(brdf, trace_color(shadow_ray, recursion_depth + 1)) * (reciever_angle * emitter_angle / dist);
                   }
                }
            }

            float weight = (recursion_depth == 0 || !next_event) ? 1 : 0.5;
            color = color + weight * (direct_light + indirect_light) / termination_prob;
        }

        //if specular reflection is used, recurse call
        if(hr.is_specular()){
            ray reflected_ray = get_reflected_ray(r, sect_pt, normal);
            return trace_color(reflected_ray, recursion_depth + 1);
        }


    }

    //if there is no hit or max recursion depth is reached return bgc
    return color;

}

inline ray get_reflected_ray(const ray &r, const vec3f sect_pt, const vec3f &normal) {
    // to reflect a ray, incident angle to normal = outgoing angle to normal
    vec3f dir_ = r.dir - 2 * (r.dir * normal) * normal;
    ray r_(sect_pt, dir_);
    return r_;
}

inline vec3f orthogonalize(const vec3f &normal, const vec3f &non_ortho) {
   float h = normal*non_ortho;
   return non_ortho - normal*h;
}

inline vec3f make_orthonormal_vector(const vec3f &normal) {
   vec3f candidates[3];
   candidates[0] = vec3f(1,0,0);
   candidates[1] = vec3f(0,1,0);
   candidates[2] = vec3f(0,0,1);

   float min = 1e20;
   unsigned cIndex = 0;
   for (unsigned i=0; i<3; i++) {
      float p = fabs(normal*candidates[i]);
      if (p<min) {
         min = p;
         cIndex = i;
      }
   }
   vec3f result = orthogonalize(normal, candidates[cIndex]);
   return result.normalized();
}

inline vec3f shift_into_normal_system(vec3f normal, vec3f dir){
    vec3f u = make_orthonormal_vector(normal);
    vec3f v = cross(normal, u);

    vec3f ret = u * dir[0] + v * dir[1] + normal * dir[2];
    return ret;
}



ray random_ray_in_hemisphere_reject(const vec3f &origin, const vec3f &normal){
    //returns random ray in hemisphere of origin  by rejection method

    //random numbers between 0-1 and 1
    float x, y, z, radius;
    vec3f dir;
    do{
        x = randf(2)-1;
        z = randf(2)-1;
        y = randf(2)-1;
        radius = std::sqrt(x*x+y*y+z*z);
    }
    while(radius > 1 || radius == 0); //check if is inside ball & correct hemisphere
    x /= radius;
    y /= radius;
    z /= radius;

    //check if dir has the correct orientation
    dir = vec3f(x,y,z);

    if(dir * normal > 0)
        return ray(origin, dir);
    else
        return ray(origin, -dir);
}

ray random_ray_in_hemisphere_constr(const vec3f &origin, const vec3f &normal){
    //returns random ray in hemisphere of origin by constructive method
    float theta, phi;
    vec3f dir;
    phi = randf(2*M_PI);
    theta = std::acos(randf(1));
    dir = vec3f(std::sin(theta) * std::cos(phi), std::sin(theta) * std::sin(phi), std::cos(theta));
    return ray(origin, shift_into_normal_system(normal, dir));
}



ray random_ray_in_hemisphere_cosw(const vec3f &origin, const vec3f &normal){
    //cosine weighted hemisphere sampling

    float radius = randf(1);
    float r = std::sqrt(radius);
    float theta = randf(2*M_PI);
    float x = r * std::cos(theta);
    float y = r * std::sin(theta);
    vec3f dir(x, y, std::sqrt(1 - radius)); // (1 - rnd) = sqrt(1-x^2-y^2))

    return ray(origin, shift_into_normal_system(normal, dir));
}


