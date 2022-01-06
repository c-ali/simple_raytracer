#ifndef SHADING_H
#define SHADING_H

#include <vector>
#include <QColor>
#include "algebra.h"
#include "util.h"

class shader{
protected:
    std::vector<vec3f> light_srcs;
    std::vector<float> light_intensites;
    vec3f viewer_pos;
    float ambient_intensity;
    void fetch_surface_color(hit_record &hr);
    virtual void apply_shading(float &ored, float &ogreen, float &oblue, const float &sred, const float &sgreen, const float &sblue, const vec3f &normal, const vec3f &light_incidence, const vec3f &sect_coords, int light_src_idx) = 0;
public:
    vec3f shade(hit_record &hr, std::vector<bool> in_shadow);
    shader(std::vector<vec3f> light_srcs, vec3f viewer_pos, std::vector<float> light_intensites, float ambient_intensity);
};

class lamb_shader : public shader{
private:
    void apply_shading(float &ored, float &ogreen, float &oblue, const float &sred, const float &sgreen, const float &sblue, const vec3f &normal, const vec3f &light_incidence, const vec3f &sect_coords, int light_src_idx) override;
public:
    lamb_shader(std::vector<vec3f> light_srcs,vec3f viewer_pos, std::vector<float> light_intensites, float ambient_intensity = 0.4);
};

class phong_shader : public shader{
private:
    int phong_exp = 100;
    std::vector<int> specular_color {100,100,100};
    void apply_shading(float &ored, float &ogreen, float &oblue, const float &sred, const float &sgreen, const float &sblue, const vec3f &normal, const vec3f &light_incidence, const vec3f &sect_coords, int light_src_idx) override;
public:
    phong_shader(std::vector<vec3f> light_srcs,vec3f viewer_pos, std::vector<float> light_intensites, float ambient_intensity = 0.4);
};

#endif // SHADING_H
