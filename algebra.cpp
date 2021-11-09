#include <cmath>
#include "algebra.h"

vec3f::vec3f(float x, float y, float z) : x(x), y(y), z(z){};
vec3f::vec3f() : x(1),y(1), z(1) {};

vec3f vec3f::operator-(){
    vec3f res(-x, -y, -z);
    return res;
}

float vec3f::norm() const{
    return std::sqrt(x*x + y*y + z*z);
}

vec3f vec3f::normalized() const{
    float n = norm();
    return vec3f(x/n, y/n, z/n);
}

vec3f operator+(vec3f first, vec3f second){
    vec3f res(first.x+second.x, first.y+second.y, first.z+second.z);
    return res;
}

vec3f operator-(vec3f first, vec3f second){
    vec3f res(first.x-second.x, first.y-second.y, first.z-second.z);
    return res;
}

vec3f operator*(vec3f vec, float scalar){
    vec3f res(vec.x*scalar, vec.y*scalar, vec.z*scalar);
    return res;
}

vec3f operator*(float scalar, vec3f vec){
    return operator*(vec, scalar);
}

vec3f operator/(vec3f vec, float scalar){
    vec3f res = operator*(vec, 1/scalar);
    return res;
}

float operator*(vec3f first, vec3f second){
    float res = first.x * second.x + first.y * second.y + first.z * second.z;
    return res;
}

vec3f cross(vec3f first, vec3f second){
    vec3f res;
    res.x = first.y * second.z - first.z * second.y;
    res.y = first.z * second.x - first.x * second.z;
    res.z = first.x * second.y - first.y * second.x;
    return res;
}


float vec3f::operator[](size_t idx){
    switch(idx){
    case 0: return x;
    case 1: return y;
    case 2: return z;
    }
}

bool operator<(vec3f first, vec3f second){
    return first.x < second.x && first.y < second.y && first.z < second.z;
}

bool operator>(vec3f first, vec3f second){
    return first.x > second.x && first.y > second.y && first.z > second.z;
}






