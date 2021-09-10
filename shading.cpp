#include <algorithm>
#include <QColor>
#include "shading.h"

shader::shader(std::vector<vec3d> light_srcs, std::vector<float> light_intensites, float ambient_intensity) : light_srcs(light_srcs), light_intensites(light_intensites), ambient_intensity(ambient_intensity){
    initialize();
};

void shader::get_surface_color(hit_record hr){
    QRgb scolor = hr.get_surface_color();
    sred = qRed(scolor);
    sgreen = qGreen(scolor);
    sblue = qBlue(scolor);
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

lamb_shader::lamb_shader(std::vector<vec3d> light_srcs, std::vector<float> light_intensites, float ambient_intensity) : shader(light_srcs, light_intensites, ambient_intensity){};

QRgb lamb_shader::shade(hit_record hr, std::vector<bool> in_shadow){
    initialize();
    get_surface_color(hr);
    add_ambient();

    //get and normalize normal vector
    vec3d normal = hr.get_normal().normalized();

    for(int i = 0; i < light_srcs.size(); ++i){
        //compute and normalize light incidence vector
        vec3d light_incidence = light_srcs[i] - hr.get_sect_coords();
        light_incidence = light_incidence.normalized();

        //apply lambertiean shading if is not in shadow
        if(!in_shadow[i]){
            float dot = std::max((float) 0, normal * light_incidence);
            ored += sred * light_intensites[i] * dot;
            oblue += sblue * light_intensites[i] * dot;
            ogreen += sgreen * light_intensites[i] * dot;
        }
    }

    //check bounds
    ored = std::min(255,ored);
    oblue = std::min(255,oblue);
    ogreen = std::min(255,ogreen);

    return qRgb(ored, ogreen, oblue);
}
