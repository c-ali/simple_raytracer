#include <cmath>
#include <memory>
#include <fstream>
#include <iostream>
#include <sstream>
#include "geometry.h"
#include "data_structures.h"

using std::make_shared, std::min, std::max;
int hit_count;
ray::ray(vec3f origin,vec3f dir) : origin(origin), dir(dir){};

void surface::set_color(QRgb color){this->color = color;};

sphere::sphere(vec3f center, float radius) : center(center), radius(radius){};

vec3f sphere::get_normal(vec3f sec_pt){
    //unit normal to the surface
    vec3f res = sec_pt - center;
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
            vec3f intersect_coord = r.origin + t_intersect * r.dir;
            rec.register_hit(get_normal(intersect_coord), intersect_coord, color, t_intersect, specular);
            return true;
        }
    }
    return false;
};

box sphere::bounding_box(){
        vec3f min = center - vec3f(radius, radius, radius);
        vec3f max = center + vec3f(radius, radius, radius);
        return box(min, max);
};



box triangle::bounding_box(){
    return box(vec3f(std::min({v1.x, v2.x, v3.x}), std::min({v1.y, v2.y, v3.y}), std::min({v1.z, v2.z, v3.z})),
               vec3f(std::max({v1.x, v2.x, v3.x}), std::max({v1.y, v2.y, v3.y}), std::max({v1.z, v2.z, v3.z})));
};


vec3f triangle::centroid(){
    return (v1+v2+v3)/3;
}

vec3f sphere::centroid(){
    return center;
}

triangle::triangle(vec3f v1, vec3f v2, vec3f v3) : v1(v1), v2(v2), v3(v3), has_normals(false){}

triangle::triangle(vec3f v1, vec3f v2, vec3f v3, vec3f n1, vec3f n2, vec3f n3)
    : v1(v1), v2(v2), v3(v3), n1(n1), n2(n2), n3(n3), has_normals(true){}


bool triangle::hit(ray r, float t0, float t1, hit_record &rec){
    hit_count++;
    //solve linear system
    float a = v1.x - v2.x;
    float b = v1.y - v2.y;
    float c = v1.z - v2.z;
    float d = v1.x - v3.x;
    float e = v1.y - v3.y;
    float f = v1.z - v3.z;
    float g = r.dir.x;
    float h = r.dir.y;
    float i = r.dir.z;
    float j = v1.x - r.origin.x;
    float k = v1.y - r.origin.y;
    float l = v1.z - r.origin.z;

    float ak_minus_jb = a * k - j * b;
    float jc_minus_al = j * c - a * l;
    float bl_minus_kc = b * l - k * c;
    float ei_minus_hf = e * i - h * f;
    float gf_minus_di = g * f - d * i;
    float dh_minus_eg = d * h - e * g;

    float M = a * ei_minus_hf + b * gf_minus_di + c * dh_minus_eg;

    if(M == 0){
        return false;
    }

    float t = - (f * ak_minus_jb + e * jc_minus_al + d * bl_minus_kc) / M;
    if(t < t0 || t > t1)
        return false;

    float gamma = (i * ak_minus_jb + h * jc_minus_al + g * bl_minus_kc) / M;
    if(gamma < 0 || gamma > 1)
        return false;

    float beta = (j * ei_minus_hf + k * gf_minus_di + l * dh_minus_eg) / M;
    if(beta < 0 || beta > (1 - gamma))
        return false;


    //compute intersection
    vec3f intersect_coord = r.origin + t * r.dir;

    //compute normals
    vec3f normal;
    if(has_normals){
        if(interpolate_normals){
            normal = (1 - gamma - beta) * n1 + (beta) * n2 + gamma * n3;
        }
        else{
            normal = (n1 + n2 + n3);
        }
    }
    else
        normal = cross((v2 - v1),(v3 - v1));
    normal = normal.normalized();

    rec.register_hit(normal, intersect_coord, color, t, specular);
    return true;
};


checkerboard::checkerboard(float height) : height(height){};

box checkerboard::bounding_box()
{
    return box(vec3f(-1e30,-1e30,height), vec3f(1e-30,1e30,height));
}

vec3f checkerboard::centroid()
{
    return vec3f(0,height,0);
}

bool checkerboard::hit(ray r, float t0, float t1, hit_record &rec)
{
    float t_sect = (height - r.origin.y) / r.dir.y;

    if(t_sect < t0 || t_sect > t1)
        return false;

    vec3f sect_coords = r.origin + t_sect * r.dir;

    if(std::fmod(std::abs(sect_coords.x) + std::abs(sect_coords.z), 2) < 1)
    //if(std::fmod(sect_coords.x + sect_coords.z, 5) < 2.5)
        color = qRgb(255,255,255);
    else
        color = qRgb(0,0,0);
    vec3f normal(0,1,0);
    rec.register_hit(normal, sect_coords , color, t_sect, specular);
    return true;
}


bool mesh::hit(ray r, float t0, float t1, hit_record &rec){
    if(!(basic_tree || fast_tree))
        return mesh::hit_without_tree(r, t0, t1, rec);
    if(fast_tree){
        return mesh::hit_with_tree(fast_tree, r, t0, t1, rec);
    }
    if(basic_tree)
        return mesh::hit_with_tree(basic_tree, r, t0, t1, rec);
}

bool mesh::hit_without_tree(ray r, float t0, float t1, hit_record &rec){
    bool hit = false;

    float t = t1;

    for(size_t i = 0; i < faces.size(); ++i){
        if(faces.at(i)->hit(r, t0, t, rec)){
            hit = true;
            t = rec.get_t();
        }
    }
    return hit;
}

bool mesh::hit_with_tree(std::shared_ptr<basic_kd_tree> &node,ray r, float t0, float t1, hit_record &rec){
    bool hit = false;
    float t = t1;

    //if node is a leaf node
    for(size_t i = 0; i < node->node_objs.size(); ++i){
        if(node->node_objs.at(i)->hit(r, t0, t, rec)){
            hit = true;
            t = rec.get_t();
        }
    }

    //else recurse
    if(node->lower && intersects(node->upper_split_bbox, r)){
        if(mesh::hit_with_tree(node->upper, r, t0, t, rec)){
            hit = true;
            t = rec.get_t();
        }
    }

    if(node->upper  && intersects(node->lower_split_bbox, r)){
        if(mesh::hit_with_tree(node->lower, r, t0, t, rec)){
            hit = true;
            t = rec.get_t();
        }
    }

  return hit;
}

bool mesh::hit_with_tree(std::shared_ptr<fast_kd_tree> &node,ray r, float t0, float t1, hit_record &rec){
        bool hit = false;
        float t = t1;

        //if node is a leaf node
        for(size_t i = 0; i < node->node_objs.size(); ++i){
            if(node->node_objs.at(i)->hit(r, t0, t, rec)){
                hit = true;
                t = rec.get_t();
            }
        }

        // -- compute ray-split plane intersection (for overlapping nodes)
        float rayParamPlaneUpper = (node->upper_node_lower_bound - r.origin[node->split_dim]) / r.dir[node->split_dim];
        float rayParamPlaneLower = (node->lower_node_upper_bound - r.origin[node->split_dim]) / r.dir[node->split_dim];

        // -- case one - node traverses from lower to upper
        if (r.dir[node->split_dim] >= 0) {
            if (node->lower && rayParamPlaneLower >= t0) {
                if (hit_with_tree   (node->lower, r, t0, min(rec.get_t(), min(t1, rayParamPlaneLower)), rec)) hit=true;
            }

            if (node->lower && rayParamPlaneUpper < t1) {
                if (hit_with_tree   (node->upper, r, max(t0, rayParamPlaneUpper), min(rec.get_t(), t1), rec)) hit=true;
            }
        // -- case two - node traverses from upper to lower
        } else {
            if (node->lower && rayParamPlaneUpper >= t0) {
                if (hit_with_tree   (node->upper, r, t0, min(rec.get_t(), min(t1, rayParamPlaneUpper)), rec)) hit=true;
            }

            if (node->lower && rayParamPlaneLower < t1) {
                if (hit_with_tree   (node->lower, r, max(t0, rayParamPlaneLower), min(rec.get_t(), t1), rec)) hit=true;
            }
        }
       return hit;
}

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
            vec3f v;
            content>>v.x; content>>v.y; content>>v.z;
            vertices.push_back(v);
        }

        //check for normals
        if(line.substr(0,2) == "vn"){
            vec3f n;
            content>>n.x; content>>n.y; content>>n.z;
            normals.push_back(n);
            normals_present = true;
        }
        //check for faces. only works for 3d faces
        if(line.substr(0,2) == "f "){
            if(normals_present){
                std::string current_block;
                std::vector<int> idxs;
                for(int i=0; i < 3; ++i){
                    content>>current_block;
                    while(current_block.find("/") != std::string::npos){
                        int delimiter_pos = current_block.find("/");
                        if(delimiter_pos > 0)
                            idxs.push_back(std::stoi(current_block.substr(0,delimiter_pos)));
                        current_block = current_block.substr(delimiter_pos+1);
                    }
                    idxs.push_back(std::stoi(current_block));
                }
                if(idxs.size() > 6)
                    //case has textures, ignore for now
                    faces.push_back(make_shared<triangle>(vec3f(get_vertex(idxs[0])), vec3f(get_vertex(idxs[3])), vec3f(get_vertex(idxs[6])),
                                                          vec3f(get_normal(idxs[2])), vec3f(get_normal(idxs[5])), vec3f(get_normal(idxs[8]))));
                else
                    //case has no textures but normals
                    faces.push_back(make_shared<triangle>(vec3f(get_vertex(idxs[0])), vec3f(get_vertex(idxs[2])), vec3f(get_vertex(idxs[4])),
                                                      vec3f(get_normal(idxs[1])), vec3f(get_normal(idxs[3])), vec3f(get_normal(idxs[5]))));
            }
            else{
                //case has only vertices
                std::vector<int> idxs;
                int v_idx;
                for(int i=0; i < 3; ++i){
                   content>>v_idx;
                    idxs.push_back(v_idx);
                }

                faces.push_back(make_shared<triangle>(vec3f(get_vertex(idxs[0])), vec3f(get_vertex(idxs[1])), vec3f(get_vertex(idxs[2]))));
            }
        }
    }
}

box mesh::bounding_box(){
    box b = box();
    for(size_t i = 0; i < faces.size(); ++i){
        b = box_union(b, faces[i]->bounding_box());
    }
    return b;
}

void mesh::build_basic_tree(int min_node_size, int max_depth){
    basic_tree = std::make_shared<basic_kd_tree>(faces, 0, min_node_size, max_depth);
}

void mesh::build_fast_tree(int min_node_size, int max_depth){
    bbox = bounding_box();
    fast_tree = std::make_shared<fast_kd_tree>(faces, 0, min_node_size, max_depth);
}

//since objs are 1-indexed, we write a getter-function
vec3f mesh::get_vertex(int idx){
    return vertices.at(idx-1);
}

//since objs are 1-indexed, we write a getter-function
vec3f mesh::get_normal(int idx){
    return normals.at(idx-1);
}


