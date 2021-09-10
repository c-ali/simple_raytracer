#ifndef SHADING_H
#define SHADING_H

#include <vector>
#include <QColor>
#include "util.h"

class shader{
protected:
    std::vector<vec3d> light_srcs;
    float light_intensity;
    float ambient_intensity;
public:
    virtual QRgb shade(hit_record hr, std::vector<bool> in_shadow) = 0;
    shader(std::vector<vec3d> light_srcs, float light_intensity, float ambient_intensity);
};

class lamb_shader : public shader{
public:
    QRgb shade(hit_record hr, std::vector<bool> in_shadow) override; //in_shadow indicates for each light source if the object is in the shadows or not
    lamb_shader(std::vector<vec3d> light_srcs, float light_intensity = 1, float ambient_intensity = 0.3);
};

#endif // SHADING_H
