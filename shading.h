#ifndef SHADING_H
#define SHADING_H

#include <vector>
#include <QColor>
#include "util.h"

class shader{
protected:
    vec3d light_src;
    float light_intensity;
    float ambient_intensity;
public:
    virtual QRgb shade(hit_record hr, bool in_shadow) = 0;
    shader(vec3d light_src, float light_intensity, float ambient_intensity);
};

class lamb_shader : public shader{
private:
    std::vector<vec3d> light_srcs;
public:
    QRgb shade(hit_record hr, bool in_shadow) override;
    lamb_shader(vec3d light_src, float light_intensity = 1, float ambient_intensity = 0.2);
};

#endif // SHADING_H
