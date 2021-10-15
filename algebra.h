#ifndef ALGEBRA_H
#define ALGEBRA_H

#include <vector>
#include <cstddef>
#include <memory>
#include <QRgb>
#include <limits>

class vec3d {
public:
    float x, y, z;
    vec3d(float x, float y, float z);
    vec3d(); //empty constructor makes 1 vector
    vec3d operator-();
    float norm() const; //returns norm
    vec3d normalized() const; //returns normalized vector
    float operator[](size_t idx);
};

vec3d operator-(vec3d first, vec3d second);
vec3d operator+(vec3d first, vec3d second);
vec3d operator*(vec3d vec, float scalar);
vec3d operator*(float scalar, vec3d vec);
float operator*(vec3d first, vec3d second); //scalar product
vec3d operator/(vec3d vec, float scalar);
vec3d cross(vec3d first, vec3d second);


class box {
public:
    vec3d min;
    vec3d max;
    box(vec3d min, vec3d max);
};

class hit_record {
private:
    vec3d normal;
    vec3d sect_coords;
    QRgb surface_color;
    float t = std::numeric_limits<float>::max();
public:
    vec3d* get_normal();
    vec3d*  get_sect_coords();
    QRgb*  get_surface_color();
    void reset();
    hit_record();
    float get_dist();
    void register_hit(vec3d normal, vec3d sect_coords, QRgb surface_color, float t);
};

class ray {
public:
    vec3d origin, dir;
    ray(vec3d origin,vec3d dir);
};


#endif // ALGEBRA_H
