#ifndef ALGEBRA_H
#define ALGEBRA_H

#include <vector>
#include <cstddef>
#include <memory>
#include <QRgb>
#include <limits>

class vec3f {
public:
    float x, y, z;
    vec3f(float x, float y, float z);
    vec3f(); //empty constructor makes 1 vector
    vec3f operator-();
    float norm() const; //returns norm
    vec3f normalized() const; //returns normalized vector
    float operator[](size_t idx);
};

vec3f operator-(vec3f first, vec3f second);
vec3f operator+(vec3f first, vec3f second);
vec3f operator*(vec3f vec, float scalar);
vec3f operator*(float scalar, vec3f vec);
float operator*(vec3f first, vec3f second); //scalar product
vec3f operator/(vec3f vec, float scalar);
vec3f cross(vec3f first, vec3f second);
bool operator<(vec3f first, vec3f second);
bool operator>(vec3f first, vec3f second);



class box {
public:
    vec3f min;
    vec3f max;
    box(vec3f min, vec3f max);
    box();
};

class ray {
public:
    vec3f origin, dir;
    ray(vec3f origin,vec3f dir);
};

box box_union(const box &first, const box &second);
bool contains(const box &b, const vec3f &pt);
bool intersects(const box &b, const ray &r);

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

;


#endif // ALGEBRA_H
