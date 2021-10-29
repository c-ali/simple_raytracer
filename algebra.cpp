#include <cmath>
#include "algebra.h"

vec3f::vec3f(float x, float y, float z) : x(x), y(y), z(z){};
vec3f::vec3f() : x(1),y(1), z(1) {};

vec3f vec3f::operator-(){
    vec3f res(-x, -y, -z);
    return res;
}

float vec3f::norm() const{
    return std::sqrt(x*x + y*y + z*z);
}

vec3f vec3f::normalized() const{
    float n = norm();
    return vec3f(x/n, y/n, z/n);
}

vec3f operator+(vec3f first, vec3f second){
    vec3f res(first.x+second.x, first.y+second.y, first.z+second.z);
    return res;
}

vec3f operator-(vec3f first, vec3f second){
    vec3f res(first.x-second.x, first.y-second.y, first.z-second.z);
    return res;
}

vec3f operator*(vec3f vec, float scalar){
    vec3f res(vec.x*scalar, vec.y*scalar, vec.z*scalar);
    return res;
}

vec3f operator*(float scalar, vec3f vec){
    return operator*(vec, scalar);
}

vec3f operator/(vec3f vec, float scalar){
    vec3f res = operator*(vec, 1/scalar);
    return res;
}

float operator*(vec3f first, vec3f second){
    float res = first.x * second.x + first.y * second.y + first.z * second.z;
    return res;
}

vec3f cross(vec3f first, vec3f second){
    vec3f res;
    res.x = first.y * second.z - first.z * second.y;
    res.y = first.z * second.x - first.x * second.z;
    res.z = first.x * second.y - first.y * second.x;
    return res;
}


float vec3f::operator[](size_t idx){
    switch(idx){
    case 0: return x;
    case 1: return y;
    case 2: return z;
    }
}

bool operator<(vec3f first, vec3f second){
    return first.x < second.x && first.y < second.y && first.z < second.z;
}

bool operator>(vec3f first, vec3f second){
    return first.x > second.x && first.y > second.y && first.z > second.z;
}

box::box() : min(vec3f(1e30f,1e30f,1e30f)), max(vec3f(-1e30f,-1e30f,-1e30f)){};

box::box(vec3f min, vec3f max) : min(min), max(max){};

box box_union(const box &first, const box &second){
    vec3f min = vec3f(std::min(first.min.x, second.min.x),std::min(first.min.y, second.min.y),std::min(first.min.z, second.min.z));
    vec3f max = vec3f(std::max(first.max.x, second.max.x),std::max(first.max.y, second.max.y),std::max(first.max.z, second.max.z));
    return box(min, max);
}

bool contains(const box &b, const vec3f &pt){
    return pt.x < b.max.x && pt.y < b.max.y && pt.z < b.max.z && pt.x > b.min.x && pt.y > b.min.y && pt.z > b.min.z;
}

bool intersects(const box &b, const ray &r){
    float t_min_x = (b.min.x - r.origin.x) / r.dir.x;
    float t_max_x = (b.max.x - r.origin.x) / r.dir.x;
    if(t_min_x > t_max_x) std::swap(t_min_x, t_max_x);

    float t_min_y = (b.min.y - r.origin.y) / r.dir.y;
    float t_max_y = (b.max.y - r.origin.y) / r.dir.y;
    if(t_min_y > t_max_y) std::swap(t_min_y, t_max_y);

    if((t_min_x > t_max_y) || (t_min_y > t_max_x))
        return false;

    float t_min_z = (b.min.z - r.origin.z) / r.dir.z;
    float t_max_z = (b.max.z - r.origin.z) / r.dir.z;
    if(t_min_z > t_max_z) std::swap(t_min_z, t_max_z);

    if(t_min_y > t_min_x)
        t_min_x = t_min_y;

    if(t_max_y > t_max_x)
        t_max_x = t_max_y;

    if((t_min_x > t_max_z) || (t_min_z > t_max_x))
        return false;

    return true;
}

ray::ray(vec3f origin,vec3f dir) : origin(origin), dir(dir){};



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

float hit_record::get_dist(){return t;};


