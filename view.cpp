#include <iostream>
#include <math.h>
#include "view.h"
#include "algebra.h"
#include "shading.h"

view::view(int width, int height, vec3f viewer_pos, vec3f viewing_dir, mesh &msh, float viewing_dst, std::vector<vec3f> light_srcs, std::vector<float> light_intensites) :
    img_width(width), img_height(height), viewer_pos(viewer_pos), w(viewing_dir.normalized()), msh(msh), viewing_dst(viewing_dst), light_srcs(light_srcs), light_intensites(light_intensites){
    //initialize random seed
    srand (static_cast <unsigned> (time(0)));
}

float randf(float scale){
    //returns random float between 0 and 1
    return scale * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

QImage view::render(){
    int hit_count = 0;
    shdr = std::make_shared<phong_shader>(light_srcs, viewer_pos, light_intensites);
    //lamb_shader s = lamb_shader(light_srcs, viewer_pos, light_intensites);
    vec3f ray_direction, ray_origin;
    QImage img(img_width, img_height, QImage::Format_RGB16);
    //loop over pixels
    for(int i = 0; i < img_height; ++i){
        //progress
        for(int j = 0; j < img_width; ++j){
            //set default color
            QRgb rgb;
            float red_sum = 0, blue_sum = 0, green_sum = 0;
            float u_offset, v_offset;

            for(int k = 0; k < samples_per_ray; k++){
                //compute pixel coordinates. jitter if necessary
                if(false){
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
                    rgb = ray_color(light_ray, eps, max_dist, 0);
                else
                    rgb = trace_color(light_ray, eps, max_dist, 0);

                //add colors to average
                QColor color(rgb);
                red_sum += color.red();
                blue_sum += color.blue();
                green_sum += color.green();
            }


            //average over num iterations and set color
            img.setPixel(i,j,qRgb(red_sum / samples_per_ray, green_sum / samples_per_ray, blue_sum / samples_per_ray));
        }
    }
    return img;
}

QRgb view::ray_color(ray r, float t0, float t1, int recursion_depth){
    //use regular raytracing to determine the color of a pixel

    QRgb color = background_color;

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


QRgb view::trace_color(ray r, float t0, float t1, int recursion_depth){
    //use path_tracing to determine the color of a pixel
    //supports only diffuse surfaces for now

    QRgb color = background_color;
    hit_record hr = hit_record(); //view hit record

    //check if ray hits any object
    if(msh.hit(r, t0, t1, hr) && recursion_depth < max_recursion_depth){
        vec3f sect_pt = *hr.get_sect_coords();
        vec3f normal = *hr.get_normal();

        //cast new ray in hemisphere with correct probability
        ray r_new = random_ray_in_hemisphere(sect_pt, normal);
        float p = 1 / (2 * M_PI);

        //compute brdf
        float cos_angle = r_new.dir * normal;
        float brdf = 1 / M_PI;

        //recursively trace reflected light
        QRgb incoming = trace_color(r_new, t0, t1, recursion_depth + 1);

        //if specular reflection is used, recurse call
        if(hr.is_specular() && recursion_depth < max_recursion_depth){
            vec3f n = *hr.get_normal();
            vec3f dir_ = r.dir - 2 * (r.dir * n) * n;
            ray r_(*hr.get_sect_coords(), dir_);
            return trace_color(r_, eps, max_dist, recursion_depth+1);
        }

        //apply rendering eq

        return add(*hr.get_emittence(), brdf * mult(incoming, *hr.get_surface_color()) * cos_angle / p);

    }

    //if there is no hit or max recursion depth is reached return bgc
    return color;

}

ray random_ray_in_hemisphere(const vec3f &origin, const vec3f &normal){
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
    dir = vec3f(x-origin.x,y-origin.y,z-origin.z);
    if(dir * normal > 0)
        return ray(origin, dir);
    else
        return ray(origin, -dir);
}
