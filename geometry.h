#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "algebra.h"
#include "util.h"


class ray {
public:
    vec3f origin, dir;
    ray(vec3f origin,vec3f dir);
};

class surface {
protected:
    QRgb color = qRgb(0,255,0);
public:
    virtual bool hit(ray r, float t0, float t1, hit_record &rec) = 0;
    virtual box bounding_box() = 0;
    virtual vec3f centroid() = 0;
    void set_color(QRgb color);
};

class sphere : public surface {
private:
    vec3f center;
    float radius;
    vec3f get_normal(vec3f sec_pt);
    QRgb color = qRgb(0,255,0);
public:
    sphere(vec3f center, float radius);
    box bounding_box() override;
    virtual vec3f centroid() override;
    bool hit(ray r, float t0, float t1, hit_record &rec) override;
};

class triangle : public surface {
private:
    vec3f v1, v2, v3;
    vec3f n1, n2, n3;
    bool has_normals;
    bool interpolate_normals = true;
public:
    triangle(vec3f v1,vec3f v2,vec3f v3,vec3f n1,vec3f n2,vec3f n3);
    triangle(vec3f v1,vec3f v2,vec3f v3);
    box bounding_box() override;
    virtual vec3f centroid() override;
    bool hit(ray r, float t0, float t1, hit_record &rec) override;
};

class mesh {
private:
   std::vector<vec3f> vertices, normals;
   std::shared_ptr<kd_tree> tree = NULL;
   vec3f get_normal(int idx);
   vec3f get_vertex(int idx);
   bool hit_without_tree(ray r, float t0, float t1, hit_record &rec);
   bool hit_with_tree(std::shared_ptr<kd_tree> &node,ray r, float t0, float t1, hit_record &rec);

public:
    int size();
    std::vector<std::shared_ptr<surface>> faces;
    void add_surface(std::shared_ptr<surface> new_surface);
    void build_tree(int min_node_size, int max_depth);
    mesh();
    std::shared_ptr<surface> operator[](size_t idx);
    bool hit(ray r, float t0, float t1, hit_record &rec);
    void read_obj(const char* path);
};

#endif // GEOMETRY_H
