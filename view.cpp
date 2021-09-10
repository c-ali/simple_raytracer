#include <iostream>
#include "view.h"
#include "util.h"
#include "shading.h"

view::view(int width, int height, vec3d viewer_pos, mesh msh, float viewing_dst, std::vector<vec3d> light_srcs, std::vector<float> light_intensites) :
    img_width(width), img_height(height), viewer_pos(viewer_pos), msh(msh), viewing_dst(viewing_dst), light_srcs(light_srcs), light_intensites(light_intensites){}

QImage view::render(){
    phong_shader s = phong_shader(light_srcs, viewer_pos, light_intensites);
    vec3d ray_direction, ray_origin;
    QImage img(img_width, img_height, QImage::Format_RGB16);
    //loop over pixels
    for(int i = 0; i < img_height; ++i){
        //progress
        std::cout << "Computing ray " + std::to_string(i*img_height) + " / " + std::to_string(img_width*img_height) << std::endl;
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
            if(msh.hit(light_ray, 0, max_dist, hr)){
                vec3d sect_pt = hr.get_sect_coords();
                if(shadows){
                    std::vector<bool> in_shadow;
                    //check for shadow by each light source
                    for(int k = 0; k < light_srcs.size(); ++k){
                        ray shadow_ray = ray(sect_pt, light_srcs[k] - sect_pt);
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
