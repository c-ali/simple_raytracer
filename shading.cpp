#include <algorithm>
#include <QColor>
#include "shading.h"

shader::shader(vec3d light_src, float light_intensity, float ambient_intensity) : light_src(light_src), light_intensity(light_intensity), ambient_intensity(ambient_intensity){};

lamb_shader::lamb_shader(vec3d light_src, float light_intensity, float ambient_intensity) : shader(light_src, light_intensity, ambient_intensity){};

QRgb lamb_shader::shade(hit_record hr, bool in_shadow){
    QRgb scolor = hr.get_surface_color();
    int sred, sgreen, sblue; //single surface color RGB values
    sred = qRed(scolor);
    sgreen = qGreen(scolor);
    sblue = qBlue(scolor);

    int ored, ogreen, oblue; //output color RGB values
    ored = ogreen = oblue = 0;

    //ambient shading
    ored += ambient_intensity*sred;
    ogreen += ambient_intensity*sgreen;
    oblue += ambient_intensity*sblue;
    //get and normalize normal vector
    vec3d normal = hr.get_normal().normalized();

    //compute and normalize light incidence vector
    vec3d light_incidence = light_src - hr.get_sect_coords();
    light_incidence = light_incidence.normalized();

    //apply lambertiean shading if is not in shadow
    if(!in_shadow){
        float dot = std::max((float) 0, normal * light_incidence);
        ored += sred * light_intensity * dot;
        oblue += sblue * light_intensity * dot;
        ogreen += sgreen * light_intensity * dot;
    }


    //check bounds
    ored = std::min(255,ored);
    oblue = std::min(255,oblue);
    ogreen = std::min(255,ogreen);

    return qRgb(ored, ogreen, oblue);
}
