#include <cmath>
#include "algebra.h"

vec3d::vec3d(float x, float y, float z) : x(x), y(y), z(z){};
vec3d::vec3d() : x(1),y(1), z(1) {};

vec3d vec3d::operator-(){
    vec3d res(-x, -y, -z);
    return res;
}

float vec3d::norm() const{
    return std::sqrt(x*x + y*y + z*z);
}

vec3d vec3d::normalized() const{
    float n = norm();
    return vec3d(x/n, y/n, z/n);
}

vec3d operator+(vec3d first, vec3d second){
    vec3d res(first.x+second.x, first.y+second.y, first.z+second.z);
    return res;
}

vec3d operator-(vec3d first, vec3d second){
    vec3d res(first.x-second.x, first.y-second.y, first.z-second.z);
    return res;
}

vec3d operator*(vec3d vec, float scalar){
    vec3d res(vec.x*scalar, vec.y*scalar, vec.z*scalar);
    return res;
}

vec3d operator*(float scalar, vec3d vec){
    return operator*(vec, scalar);
}

vec3d operator/(vec3d vec, float scalar){
    vec3d res = operator*(vec, 1/scalar);
    return res;
}

float operator*(vec3d first, vec3d second){
    float res = first.x * second.x + first.y * second.y + first.z * second.z;
    return res;
}

vec3d cross(vec3d first, vec3d second){
    vec3d res;
    res.x = first.y * second.z - first.z * second.y;
    res.y = first.z * second.x - first.x * second.z;
    res.z = first.x * second.y - first.y * second.x;
    return res;
}

vec2d::vec2d(float x, float y) : x(x), y(y){};



box::box(vec3d min, vec3d max) : min(min), max(max){};



ray::ray(vec3d origin,vec3d dir) : origin(origin), dir(dir){};



//getter and setter for hit_record
hit_record::hit_record(){};

void hit_record::set_normal(vec3d n){normal = n;};

void hit_record::set_sect_coords(vec3d s){sect_coords = s;};

void hit_record::set_surface_color(QRgb color){surface_color = color;};

vec3d*  hit_record::get_normal(){return &normal;};
vec3d*  hit_record::get_sect_coords(){return &sect_coords;};
QRgb*  hit_record::get_surface_color(){return &surface_color;};


