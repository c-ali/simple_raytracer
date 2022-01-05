#include <algorithm>
#include <QColor>
#include <math.h>
#include "shading.h"

shader::shader(std::vector<vec3f> light_srcs,vec3f viewer_pos, std::vector<float> light_intensites, float ambient_intensity) : light_srcs(light_srcs), viewer_pos(viewer_pos), light_intensites(light_intensites), ambient_intensity(ambient_intensity){
    initialize();
};

void shader::fetch_surface_color(hit_record &hr){
    QRgb *scolor = hr.get_surface_color();
    sred = qRed(*scolor);
    sgreen = qGreen(*scolor);
    sblue = qBlue(*scolor);
}

void shader::initialize(){
    ored = ogreen = oblue = 0;
}

void shader::add_ambient(){
    //ambient shading
    ored += ambient_intensity*sred;
    ogreen += ambient_intensity*sgreen;
    oblue += ambient_intensity*sblue;
}

void shader::apply_bounds(){
    ored = std::fmin(255,ored);
    oblue = std::fmin(255,oblue);
    ogreen = std::fmin(255,ogreen);
}

vec3f shader::shade(hit_record &hr, std::vector<bool> in_shadow){
    initialize();
    fetch_surface_color(hr);
    add_ambient();

    //get and normalize normal vector
    vec3f normal = hr.get_normal()->normalized();

    for(size_t i = 0; i < light_srcs.size(); ++i){
        //compute and normalize light incidence vector
        vec3f light_incidence = light_srcs[i] - *hr.get_sect_coords();
        light_incidence = light_incidence.normalized();

        //apply shading if is not in shadow
        if(!in_shadow[i]){
            apply_shading(normal, light_incidence, *hr.get_sect_coords(), i);
        }
    }

    //check bounds
    apply_bounds();

    return vec3f(ored, ogreen, oblue);
}

void lamb_shader::apply_shading(vec3f normal, vec3f light_incidence, vec3f sect_coords, int light_src_idx){
    //uses lambertial model for shading
    float dot = std::max((float) 0, normal * light_incidence);
    ored += sred * light_intensites[light_src_idx] * dot;
    oblue += sblue * light_intensites[light_src_idx] * dot;
    ogreen += sgreen * light_intensites[light_src_idx] * dot;
}

lamb_shader::lamb_shader(std::vector<vec3f> light_srcs, vec3f viewer_pos, std::vector<float> light_intensites, float ambient_intensity) : shader(light_srcs, viewer_pos, light_intensites, ambient_intensity){};

phong_shader::phong_shader(std::vector<vec3f> light_srcs, vec3f viewer_pos, std::vector<float> light_intensites, float ambient_intensity) : shader(light_srcs, viewer_pos, light_intensites, ambient_intensity){};

void phong_shader::apply_shading(vec3f normal, vec3f light_incidence, vec3f sect_coords, int light_src_idx){
    vec3f h = viewer_pos - sect_coords;
    h = h.normalized();
    h = light_incidence + h;
    h = h.normalized();
    float dot1 = std::max((float) 0, normal * light_incidence);
    float dot2 = std::max((float) 0, normal * h);
    ored += sred * light_intensites[light_src_idx] * dot1 + specular_color[0] * light_intensites[light_src_idx] * std::pow(dot2, phong_exp);
    oblue += sblue * light_intensites[light_src_idx] * dot1 + specular_color[1] * light_intensites[light_src_idx] * std::pow(dot2, phong_exp);
    ogreen += sgreen *  light_intensites[light_src_idx] * dot1 + specular_color[2] * light_intensites[light_src_idx] * std::pow(dot2, phong_exp);
}
