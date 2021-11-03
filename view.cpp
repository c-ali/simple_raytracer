#include <iostream>
#include "view.h"
#include "algebra.h"
#include "shading.h"

view::view(int width, int height, vec3f viewer_pos, vec3f viewing_dir, mesh &msh, float viewing_dst, std::vector<vec3f> light_srcs, std::vector<float> light_intensites) :
    img_width(width), img_height(height), viewer_pos(viewer_pos), w(viewing_dir), msh(msh), viewing_dst(viewing_dst), light_srcs(light_srcs), light_intensites(light_intensites){}

QImage view::render(){
    //phong_shader s = phong_shader(light_srcs, viewer_pos, light_intensites);
    lamb_shader s = lamb_shader(light_srcs, viewer_pos, light_intensites);
    vec3f ray_direction, ray_origin;
    QImage img(img_width, img_height, QImage::Format_RGB16);
    //loop over pixels
    for(int i = 0; i < img_height; ++i){
        //progress
        for(int j = 0; j < img_width; ++j){
            //compute pixel coordinates
            float u_offset = -1 + 2 * (i + 0.5) / img_width;
            float v_offset = -1 + 2 * (j + 0.5) / img_height;

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

            //set default color
            img.setPixelColor(i, j, "white");

            ray light_ray = ray(ray_origin, ray_direction);
            hit_record hr = hit_record(); //view hit record
            hit_record sr = hit_record(); //shadow record

            //check if ray hits any objec
            if(msh.hit(light_ray, eps, max_dist, hr)){
                vec3f const *sect_pt = hr.get_sect_coords();
                if(shadows){
                    std::vector<bool> in_shadow;
                    //check for shadow by each light source
                    for(size_t k = 0; k < light_srcs.size(); ++k){
                        vec3f ray_dir = light_srcs[k] - *sect_pt;
                        ray shadow_ray = ray(*sect_pt, ray_dir);
                        in_shadow.push_back(msh.hit(shadow_ray, eps, max_dist, sr));
                    }
                    img.setPixel(i,j,s.shade(hr, in_shadow));
                }
                else
                    img.setPixel(i,j,s.shade(hr, std::vector<bool>(light_srcs.size(),false)));
            }
        }
    }
    return img;
}
