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
vec3d cross(vec3d first, vec3d second);

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

class surface {
private:
    virtual vec3d get_normal(vec3d sec_pt) = 0; //gets unit normal to the surface
public:
    virtual bool hit(ray r, float t0, float t1, hit_record &rec) = 0;
    virtual box bounding_box() = 0;
};

class sphere : public surface {
private:
    vec3d center;
    float radius;
    vec3d get_normal(vec3d sec_pt) override;
    QRgb color = qRgb(0,255,0);
public:
    sphere(vec3d center, float radius);
    box bounding_box() override;
    bool hit(ray r, float t0, float t1, hit_record &rec) override;
};

class triangle : public surface {
private:
    vec3d v1, v2, v3;
    vec3d n1, n2, n3;
    QRgb color =  qRgb(0,255,0);
    vec3d get_normal(vec3d sec_pt) override;
public:
    triangle(vec3d v1,vec3d v2,vec3d v3,vec3d n1,vec3d n2,vec3d n3);
    triangle(vec3d v1,vec3d v2,vec3d v3);
    box bounding_box() override;
    bool hit(ray r, float t0, float t1, hit_record &rec) override;
};

class mesh {
private:
   std::vector<std::shared_ptr<surface>> vertices;
public:
    int size();
    void add_surface(std::shared_ptr<surface> new_surface);
    mesh();
    std::shared_ptr<surface> operator[](size_t idx);
    bool hit(ray r, float t0, float t1, hit_record &rec);
};

#endif // UTIL_H
