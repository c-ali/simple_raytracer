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
    vec3f(QRgb col); //convert color to float for more precision
    vec3f operator-();
    float norm() const; //returns norm
    vec3f normalized() const; //returns normalized vector
    float operator[](size_t idx);
    vec3f bounded() const; //returns 255-bounded vector
};

vec3f operator-(const vec3f &first, const vec3f &second);
vec3f operator+(const vec3f &first, const vec3f &second);
vec3f operator*(const vec3f &vec, float scalar);
vec3f operator*(float scalar, const vec3f &vec);
float operator*(const vec3f &first, const vec3f &second); //scalar product
vec3f operator/(const vec3f &vec, float scalar);
vec3f cross(const vec3f &first, const vec3f &second);
bool operator<(const vec3f &first, const vec3f &second);
bool operator>(const vec3f &first, const vec3f &second);
vec3f pt_mult(const vec3f &first, const vec3f &second); //pointwise multiplication

#endif // ALGEBRA_H
