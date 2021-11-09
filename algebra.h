#ifndef ALGEBRA_H
#define ALGEBRA_H

#include <vector>
#include <cstddef>
#include <memory>
#include <QRgb>
#include <limits>

class vec3f {
public:
    float x, y, z;
    vec3f(float x, float y, float z);
    vec3f(); //empty constructor makes 1 vector
    vec3f operator-();
    float norm() const; //returns norm
    vec3f normalized() const; //returns normalized vector
    float operator[](size_t idx);
};

vec3f operator-(vec3f first, vec3f second);
vec3f operator+(vec3f first, vec3f second);
vec3f operator*(vec3f vec, float scalar);
vec3f operator*(float scalar, vec3f vec);
float operator*(vec3f first, vec3f second); //scalar product
vec3f operator/(vec3f vec, float scalar);
vec3f cross(vec3f first, vec3f second);
bool operator<(vec3f first, vec3f second);
bool operator>(vec3f first, vec3f second);


#endif // ALGEBRA_H
