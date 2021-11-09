#ifndef UTIL_H
#define UTIL_H

#include "algebra.h"

class kd_tree;
class ray;
class surface;

class box {
public:
    vec3f min;
    vec3f max;
    box(vec3f min, vec3f max);
    box();
};


box box_union(const box &first, const box &second);
bool contains(const box &b, const vec3f &pt);
bool intersects(const box &b,const ray &r);


class hit_record {
private:
    vec3f normal;
    vec3f sect_coords;
    QRgb surface_color;
    float t = std::numeric_limits<float>::max();
public:
    vec3f* get_normal();
    vec3f*  get_sect_coords();
    QRgb*  get_surface_color();
    void reset();
    hit_record();
    float get_dist();
    void register_hit(vec3f normal, vec3f sect_coords, QRgb surface_color, float t);
};




#endif // UTIL_H