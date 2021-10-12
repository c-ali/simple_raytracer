#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "algebra.h"

class surface {
public:
    virtual bool hit(ray r, float t0, float t1, hit_record &rec) = 0;
    virtual box bounding_box() = 0;
};

class sphere : public surface {
private:
    vec3d center;
    float radius;
    vec3d get_normal(vec3d sec_pt);
    QRgb color = qRgb(0,255,0);
public:
    sphere(vec3d center, float radius);
    box bounding_box() override;
    bool hit(ray r, float t0, float t1, hit_record &rec) override;
};

class triangle : public surface {
private:
    QRgb color = qRgb(0,255,0);
    vec3d v1, v2, v3;
    vec3d n1, n2, n3;
    bool has_normals;
    bool interpolate_normals = true;
public:
    triangle(vec3d v1,vec3d v2,vec3d v3,vec3d n1,vec3d n2,vec3d n3);
    triangle(vec3d v1,vec3d v2,vec3d v3);
    box bounding_box() override;
    bool hit(ray r, float t0, float t1, hit_record &rec) override;
};

class mesh {
private:
   std::vector<std::shared_ptr<surface>> faces;
   std::vector<vec3d> vertices, normals;
   vec3d get_normal(int idx);
   vec3d get_vertex(int idx);
public:
    int size();
    void add_surface(std::shared_ptr<surface> new_surface);
    mesh();
    std::shared_ptr<surface> operator[](size_t idx);
    bool hit(ray r, float t0, float t1, hit_record &rec);
    void read_obj(const char* path);
};

#endif // GEOMETRY_H
