#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include "util.h"

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
    std::shared_ptr<vec3d> v1, v2, v3;
    std::shared_ptr<vec3d> n1, n2, n3;
    bool has_normals;
    vec3d get_normal(vec3d sec_pt) override;
public:
    triangle(std::shared_ptr<vec3d> v1,std::shared_ptr<vec3d> v2,std::shared_ptr<vec3d> v3,std::shared_ptr<vec3d> n1,std::shared_ptr<vec3d> n2,std::shared_ptr<vec3d> n3);
    triangle(std::shared_ptr<vec3d> v1,std::shared_ptr<vec3d> v2,std::shared_ptr<vec3d> v3);
    box bounding_box() override;
    bool hit(ray r, float t0, float t1, hit_record &rec) override;
};

class mesh {
private:
   std::vector<std::shared_ptr<surface>> faces;
   std::vector<vec3d> vertices, normals;
public:
    int size();
    void add_surface(std::shared_ptr<surface> new_surface);
    mesh();
    std::shared_ptr<surface> operator[](size_t idx);
    bool hit(ray r, float t0, float t1, hit_record &rec);
    void read_obj(const char* path);
};

#endif // DATA_STRUCTURES_H
