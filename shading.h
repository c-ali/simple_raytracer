#ifndef SHADING_H
#define SHADING_H

#include <vector>
#include <QColor>
#include "util.h"

class shader{
protected:
    std::vector<vec3d> light_srcs;
    std::vector<float> light_intensites;
    vec3d viewer_pos;
    float ambient_intensity;
    int sred, sgreen, sblue; //surface color RGB values
    int ored, ogreen, oblue; //output rgb values
    void get_surface_color(hit_record hr);
    void initialize(); //resets output values
    void add_ambient(); //adds ambient shading
    void apply_bounds(); //makes sure rgb values are not over 255
    virtual void apply_shading(vec3d normal, vec3d light_incidence, vec3d sect_coords, int light_src_idx) = 0;
public:
    QRgb shade(hit_record hr, std::vector<bool> in_shadow);
    shader(std::vector<vec3d> light_srcs, vec3d viewer_pos, std::vector<float> light_intensites, float ambient_intensity);
};

class lamb_shader : public shader{
private:
    void apply_shading(vec3d normal, vec3d light_incidence, vec3d sect_coords, int light_src_idx) override;
public:
    lamb_shader(std::vector<vec3d> light_srcs,vec3d viewer_pos, std::vector<float> light_intensites, float ambient_intensity = 0.3);
};

class phong_shader : public shader{
private:
    int phong_exp = 3;
    std::vector<int> specular_color {100,100,100};
    void apply_shading(vec3d normal, vec3d light_incidence, vec3d sect_coords, int light_src_idx) override;
public:
    phong_shader(std::vector<vec3d> light_srcs,vec3d viewer_pos, std::vector<float> light_intensites, float ambient_intensity = 0.3);
};

#endif // SHADING_H
