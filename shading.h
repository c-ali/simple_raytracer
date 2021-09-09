#ifndef SHADING_H
#define SHADING_H

#include <vector>
#include <QColor>
#include "util.h"

class shader{
protected:
    vec3d light_src;
    float light_intensity;
public:
    virtual QRgb shade(hit_record hr) = 0;
    shader(vec3d light_src, float light_intensity);
};

class lamb_shader : public shader{
private:
    std::vector<vec3d> light_srcs;
public:
    QRgb shade(hit_record hr) override;
    lamb_shader(vec3d light_src, float light_intensity);
};

#endif // SHADING_H
