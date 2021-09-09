#include <cmath>
#include "util.h"

vec3d::vec3d(float x, float y, float z) : x(x), y(y), z(z){};
vec3d::vec3d() : x(1),y(1), z(1) {};

vec3d vec3d::operator-(){
    vec3d res(-x, -y, -z);
    return res;
}

float vec3d::norm(){
    return std::sqrt(x*x + y*y + z*z);
}

vec3d vec3d::normalized(){
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

vec2d::vec2d(float x, float y) : x(x), y(y){};

box::box(vec3d min, vec3d max) : min(min), max(max){};

ray::ray(vec3d origin, vec3d dir) : origin(origin), dir(dir){};

//getter and setter for hit_record
hit_record::hit_record(){};
hit_record::hit_record(float t0, vec3d normal) : t0(t0), normal(normal){};
void hit_record::set_normal(vec3d n){normal = n;};
vec3d hit_record::get_normal(){return normal;}
void hit_record::set_sect_coords(vec3d coords){sect_coords = coords;};
vec3d hit_record::get_sect_coords(){return sect_coords;};
void hit_record::set_surface_color(QRgb color){surface_color = color;};
QRgb hit_record::get_surface_color(){return surface_color;};

bool mesh::hit(ray r, float t0, float t1, hit_record &rec){
    for(int i = 0; i < vertices.size(); ++i){
        if(vertices.at(i)->hit(r, t0, t1, rec))
            return true;
    }
    return false;
}

sphere::sphere(vec3d center, float radius) : center(center), radius(radius){};

vec3d sphere::get_normal(vec3d sec_pt){
    //unit normal to the surface
    vec3d res = sec_pt - center;
    return res;
}


bool sphere::hit(ray r, float t0, float t1, hit_record &rec){
    //Sphere hit equation is a quadratic form
    //At^2 + Bt + C = 0
    float A = r.dir * r.dir;
    float B = r.dir*(r.origin-center)*2;
    float C = (r.origin - center) * (r.origin - center) - radius*radius;

    //first compute determinant if intersection is even there
    float det = B*B-4*A*C;
    if(det > 0){
        //solution of quadratic form
        float t_intersect = (-B - std::abs(std::sqrt(det))) / (2*A);
        //check if intersection is in the interval [t0,t1]
        if(t0 < t_intersect && t_intersect < t1){
            //fill hit record and report hit
            //get section coordinate, normal and color
            vec3d intersect_coord = r.origin + t_intersect * r.dir;
            rec.set_sect_coords(intersect_coord);
            rec.set_normal(get_normal(intersect_coord));
            rec.set_surface_color(color);

            return true;
        }
    }
    return false;
};


box sphere::bounding_box(){
    vec3d min = center - vec3d(radius, radius, radius);
    vec3d max = center + vec3d(radius, radius, radius);
    return box(min, max);
}

box triangle::bounding_box(){
    //TODO
    return box(vec3d(), vec3d());
}

mesh::mesh(){}

void mesh::add_surface(std::shared_ptr<surface> new_surface){
    vertices.push_back(new_surface);
}

int mesh::size(){
    return vertices.size();
}

std::shared_ptr<surface> mesh::operator[](size_t idx){
    return vertices.at(idx);
}

