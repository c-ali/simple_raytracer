#include "util.h"
#include "data_structures.h"
#include "geometry.h"


//getter and setter for hit_record
hit_record::hit_record(){};

void hit_record::register_hit(vec3f normal, vec3f sect_coords, QRgb surface_color, float t){
    this->normal = normal;
    this->sect_coords = sect_coords;
    this->surface_color = surface_color;
    this->t = t;
}

void hit_record::reset(){
    t = std::numeric_limits<float>::max();
}

vec3f*  hit_record::get_normal(){return &normal;};

vec3f*  hit_record::get_sect_coords(){return &sect_coords;};

QRgb*  hit_record::get_surface_color(){return &surface_color;};

float hit_record::get_t(){return t;};


box::box() : min(vec3f(1e30f,1e30f,1e30f)), max(vec3f(-1e30f,-1e30f,-1e30f)){};

box::box(vec3f min, vec3f max) : min(min), max(max){};

box box_union(const box &first, const box &second){
    vec3f min = vec3f(std::min(first.min.x, second.min.x),std::min(first.min.y, second.min.y),std::min(first.min.z, second.min.z));
    vec3f max = vec3f(std::max(first.max.x, second.max.x),std::max(first.max.y, second.max.y),std::max(first.max.z, second.max.z));
    return box(min, max);
}

