#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <cstddef>
#include <memory>
#include <QRgb>

class vec3d {
public:
    float x, y, z;
    vec3d(float x, float y, float z);
    vec3d(); //empty constructor makes 1 vector
    vec3d operator-();
    float norm() const; //returns norm
    vec3d normalized() const; //returns normalized vector
};

vec3d operator-(vec3d first, vec3d second);
vec3d operator+(vec3d first, vec3d second);
vec3d operator*(vec3d vec, float scalar);
vec3d operator*(float scalar, vec3d vec);
float operator*(vec3d first, vec3d second); //scalar product
vec3d operator/(vec3d vec, float scalar);


class vec2d {
private:
    float x, y;
public:
    vec2d(float x, float y);
};

class box {
private:
    vec3d min;
    vec3d max;
public:
    box(vec3d min, vec3d max);
};

class hit_record {
private:
    vec3d normal;
    vec3d sect_coords;
    QRgb surface_color;
public:
    void set_normal(vec3d normal);
    vec3d* get_normal();
    void set_sect_coords(vec3d sect_coords);
    vec3d*  get_sect_coords();
    void set_surface_color(QRgb color);
    QRgb*  get_surface_color();
    hit_record();
};

class ray {
public:
    vec3d origin, dir;
    ray(vec3d origin,vec3d dir);
};



#endif // UTIL_H
