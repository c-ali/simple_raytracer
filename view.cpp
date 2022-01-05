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
    std::list<std::future<void>> futures;
    img = QImage(img_width, img_height, QImage::Format_RGB32);
    //loop over pixels
    for(int i = 0; i < img_height; ++i){
        //progress
//           futures.push_back(std::async(std::launch::async, &view::compute_line, std::ref(*this), i));
             std::async(std::launch::async, &view::compute_line, std::ref(*this), i);

    }

    //wait for all threads
    for(int i = 0; i < futures.size(); ++i){
        futures.back().wait();
        futures.pop_back();
    }
    return img;
}

void view::compute_line(int i){
    for(int j = 0; j < img_width; ++j){
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
        sum = sum / 10;
        img.setPixel(i,j, qRgb(sum[0], sum[1], sum[2]));
    }
}

vec3f view::ray_color(ray r, float t0, float t1, int recursion_depth){
    //use regular raytracing to determine the color of a pixel

    vec3f color = background_color;

    hit_record hr = hit_record(); //view hit record
    hit_record sr = hit_record(); //shadow record

    //check if ray hits any object
    if(msh.hit(r, t0, t1, hr)){
        vec3f const *sect_pt = hr.get_sect_coords();
        if(shadows){
            std::vector<bool> in_shadow;
            //check for shadow by each light source
            for(size_t k = 0; k < light_srcs.size(); ++k){
                vec3f ray_dir = light_srcs[k] - *sect_pt;
                ray shadow_ray = ray(*sect_pt, ray_dir);
                in_shadow.push_back(msh.hit(shadow_ray, t0, t1, sr));
            }
            color = shdr->shade(hr, in_shadow);
        }
        else
            color = shdr->shade(hr, std::vector<bool>(light_srcs.size(),false));


        //if specular reflection is used, recurse call
        if(hr.is_specular() && recursion_depth < max_recursion_depth){
            vec3f n = *hr.get_normal();
            vec3f dir_ = r.dir - 2 * (r.dir * n) * n;
            ray r_(*hr.get_sect_coords(), dir_);
            return ray_color(r_, eps, max_dist, recursion_depth+1);
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
    if(msh.hit(r, eps, max_dist, hr) && recursion_depth <= max_recursion_depth){
        vec3f normal = *hr.get_normal();

        //cast new ray in hemisphere with correct probability
        ray r_new = random_ray_in_hemisphere_constr(*hr.get_sect_coords(), normal);

        //compute brdf;
        float cos_angle = r_new.dir * normal;
        vec3f brdf = vec3f(*hr.get_surface_color()) / M_PI;

        //recursively trace reflected light
        vec3f incoming = trace_color(r_new, recursion_depth + 1);

        //if specular reflection is used, recurse call
//        if(hr.is_specular() && recursion_depth < max_recursion_depth){
//            vec3f n = *hr.get_normal();
//            vec3f dir_ = r.dir - 2 * (r.dir * n) * n;
//            ray r_(*hr.get_sect_coords(), dir_);
//            return trace_color(r_, recursion_depth+1);
//        }

        //apply rendering eq
        return vec3f(*hr.get_emittence()) + (pt_mult(brdf, incoming) *  (cos_angle / p_diffuse));

    }

    //if there is no hit or max recursion depth is reached return bgc
    return color;

}

ray random_ray_in_hemisphere_reject(const vec3f &origin, const vec3f &normal){
    //returns random ray in hemisphere of origin

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
    //random numbers between 0-1 and 1
    float theta, phi;
    vec3f dir;
    phi = randf(2*M_PI*1000)/1000;
    theta = randf(M_PI*1000)/1000;
    dir = vec3f(std::sin(theta) * std::cos(phi), std::sin(theta) * std::sin(phi), std::cos(theta));
    if(dir * normal > 0)
        return ray(origin, dir);
    else
        return ray(origin, -dir);
}
