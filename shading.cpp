#include <algorithm>
#include <QColor>
#include "shading.h"

shader::shader(vec3d light_src, float light_intensity = 1) : light_src(light_src), light_intensity(light_intensity){};

lamb_shader::lamb_shader(vec3d light_src, float light_intensity) : shader(light_src, light_intensity){};

QRgb lamb_shader::shade(hit_record hr){
    int c = 3;
    int red, green, blue;
    QRgb scolor = hr.get_surface_color();

    //get and normalize normal vector
    vec3d normal = hr.get_normal();
    normal = normal / normal.norm();

    //compute and normalize light incidence vector
    vec3d light_incidence = light_src - hr.get_sect_coords();
    light_incidence = light_incidence / light_incidence.norm();

    float dot = std::max((float) 0, normal * light_incidence);
    red = qRed(scolor);
    green = qGreen(scolor);
    blue = qGreen(scolor);
    red = red * light_intensity * dot + c;
    blue = blue * light_intensity * dot + c;
    green = green * light_intensity * dot + c;
    return qRgb(red, blue, green);
}
