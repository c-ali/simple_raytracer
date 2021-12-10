#include <iostream>
#include "view.h"
#include "algebra.h"
#include "shading.h"

view::view(int width, int height, vec3f viewer_pos, vec3f viewing_dir, mesh &msh, float viewing_dst, std::vector<vec3f> light_srcs, std::vector<float> light_intensites) :
    img_width(width), img_height(height), viewer_pos(viewer_pos), w(viewing_dir.normalized()), msh(msh), viewing_dst(viewing_dst), light_srcs(light_srcs), light_intensites(light_intensites){
    //initialize random seed
    srand (static_cast <unsigned> (time(0)));
}

float randf(float scale){
    return scale*2*static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

QImage view::render(){
    int hit_count = 0;
    float focal_offset = (focal_dist - viewing_dst) / focal_dist ; //calculate once, needed for focal dist
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
                    float scale = 0.02;
                    float rand_u_offset = randf(scale);
                    float rand_v_offset = randf(scale);
                    ray_origin = ray_origin - (u * rand_u_offset + v * rand_v_offset);
                    ray_direction = ray_direction + u * rand_u_offset * focal_offset + v * rand_v_offset * focal_offset;
                }


                ray light_ray = ray(ray_origin, ray_direction);

                rgb = ray_color(light_ray, eps, max_dist, 0);

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
