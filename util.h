#ifndef UTIL_H
#define UTIL_H

#include "algebra.h"

class ray;

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
bool intersectBox(const ray &ray, const box &bb, float &firstHitParam, float &secondHitParam);

class hit_record {
private:
    vec3f normal;
    vec3f sect_coords;
    QRgb surface_color;
    QRgb specular;
    QRgb emittence;  //for path tracing materials can emit and light_srcs are not needed

public:
    float t = 1e10f;
    vec3f* get_normal();
    vec3f*  get_sect_coords();
    QRgb*  get_surface_color();
    QRgb* get_emittence();
    bool is_specular();
    void reset();
    hit_record();
    float get_t();
    void register_hit(vec3f normal, vec3f sect_coords, QRgb surface_color, float t, bool specular, float emittence);
};


QRgb add(const QRgb &first, const QRgb &second);
QRgb mult(const QRgb &first, const QRgb &second);


#endif // UTIL_H
