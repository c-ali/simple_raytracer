#include <QColor>
#include "util.h"
#include "data_structures.h"
#include "geometry.h"




hit_record::hit_record(){};

void hit_record::register_hit(vec3f normal, vec3f sect_coords, QRgb surface_color, float t, bool specular, QRgb emittence){
    this->normal = normal;
    this->sect_coords = sect_coords;
    this->surface_color = surface_color;
    this->t = t;
    this->specular = specular;
    this->emittence = emittence;
}

vec3f hit_record::brdf(float reciever_angle)
{

}



void hit_record::reset(){
    t = std::numeric_limits<float>::max();
}

vec3f*  hit_record::get_normal(){return &normal;};

vec3f*  hit_record::get_sect_coords(){return &sect_coords;};

QRgb*  hit_record::get_surface_color(){return &surface_color;};

QRgb* hit_record::get_emittence(){return &emittence;};

float hit_record::get_t(){return t;};

bool hit_record::is_specular(){return specular;};


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

    if (t_min_z > t_min_x)
         t_min_x = t_min_z;

     if (t_max_z < t_max_x)
         t_max_x = t_max_z;

    if((t_min_x > t_max_z) || (t_min_z > t_max_x))
        return false;

    return true;
}

QRgb add(const QRgb &first, const QRgb &second){
    return qRgb(qRed(first) + qRed(second), qGreen(first) + qGreen(second), qBlue(first) + qBlue(second));
}

QRgb mult(const QRgb &first, const QRgb &second){
    return qRgb(qRed(first) * qRed(second), qGreen(first) * qGreen(second), qBlue(first) * qBlue(second));
}

QRgb mult(const QRgb &first, const float scalar){
    return qRgb(qRed(first) * scalar, qGreen(first) * scalar, qBlue(first) * scalar);
}

QRgb div(const QRgb &first, const QRgb &second){
    return qRgb(qRed(first) / qRed(second), qGreen(first) / qGreen(second), qBlue(first) / qBlue(second));
}

QRgb div(const QRgb &first, const float scalar){
    return qRgb(qRed(first) / scalar, qGreen(first) / scalar, qBlue(first) / scalar);
}
