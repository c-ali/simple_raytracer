#include <cmath>
#include <memory>
#include <fstream>
#include <iostream>
#include <sstream>
#include "data_structures.h"

using std::make_shared;

bool mesh::hit(ray r, float t0, float t1, hit_record &rec){
    for(size_t i = 0; i < faces.size(); ++i){
        if(faces.at(i)->hit(r, t0, t1, rec))
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
};



box triangle::bounding_box(){
    //TODO
    return box(vec3d(), vec3d());
};

triangle::triangle(std::shared_ptr<vec3d> v1, std::shared_ptr<vec3d> v2, std::shared_ptr<vec3d> v3) : v1(v1), v2(v2), v3(v3), has_normals(false){}

triangle::triangle(std::shared_ptr<vec3d> v1, std::shared_ptr<vec3d> v2, std::shared_ptr<vec3d> v3, std::shared_ptr<vec3d> n1, std::shared_ptr<vec3d> n2, std::shared_ptr<vec3d> n3)
    : v1(v1), v2(v2), v3(v3), n1(n1), n2(n2), n3(n3), has_normals(true){}

vec3d triangle::get_normal(vec3d sec_pt){
    vec3d normal = (*n1 + *n2 + *n3)/3;
    return normal;
};

bool triangle::hit(ray r, float t0, float t1, hit_record &rec){}

mesh::mesh(){}

void mesh::add_surface(std::shared_ptr<surface> new_surface){
    faces.push_back(new_surface);
}

int mesh::size(){
    return faces.size();
}

std::shared_ptr<surface> mesh::operator[](size_t idx){
    return faces.at(idx);
}

void mesh::read_obj(const char* path){
    std::ifstream in(path, std::ios::in);

    if (!in)
        {
            std::cerr << "Cannot find file " << path << std::endl;
            exit(1);

        }

    std::string line;
    bool normals_present = false;
    while(std::getline(in, line)){
        if(line.size() < 2){
            continue;
        }

        std::istringstream content(line.substr(2));
        //check for vertices
        if(line.substr(0,2) == "v "){
            vec3d v;
            content>>v.x; content>>v.y; content>>v.z;
            vertices.push_back(v);
        }

        //check for normals
        if(line.substr(0,2) == "vn"){
            vec3d n;
            content>>n.x; content>>n.y; content>>n.z;
            normals.push_back(n);
            normals_present = true;
        }
        //check for faces. only works for 3d faces
        if(line.substr(0,2) == "f "){
            if(normals_present){
                std::string current_block;
                int delimiter_pos;
                std::vector<int> v_idxs, n_idxs;
                for(int i=0; i < 3; ++i){
                    content>>current_block;
                    delimiter_pos = current_block.find("//");
                    v_idxs.push_back(std::stoi(current_block.substr(0,delimiter_pos)));
                    n_idxs.push_back(std::stoi(current_block.substr(delimiter_pos+2, current_block.size())));
                }
                faces.push_back(make_shared<triangle>(make_shared<vec3d>(vertices[v_idxs[0]]), make_shared<vec3d>(vertices[v_idxs[1]]), make_shared<vec3d>(vertices[v_idxs[2]]),
                                                      make_shared<vec3d>(normals[n_idxs[0]]), make_shared<vec3d>(normals[n_idxs[1]]), make_shared<vec3d>(normals[n_idxs[2]])));
            }
            else{
                std::vector<int> v_idxs;
                int v_idx;
                for(int i=0; i < 3; ++i){
                   content>>v_idx;
                    v_idxs.push_back(v_idx);
                }

                faces.push_back(make_shared<triangle>(make_shared<vec3d>(vertices[v_idxs[0]]), make_shared<vec3d>(vertices[v_idxs[1]]), make_shared<vec3d>(vertices[v_idxs[2]])));
            }
        }
    }
    int x = 5;
}
