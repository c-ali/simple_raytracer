#ifndef VIEW_H
#define VIEW_H

#include <vector>
#include <QImage>
#include <memory>
#include <QColor>
#include "algebra.h"
#include "geometry.h"
#include "shading.h"

extern int hit_count;


class view
{
private:
    int img_width, img_height;
    vec3f viewer_pos;
    mesh msh;
    float viewing_dst;
    std::vector<vec3f> light_srcs;
    bool shadows = true;
    std::vector<float> light_intensites;
    int max_recursion_depth = 5;

    vec3f u = vec3f(1,0,0); //right
    vec3f v = vec3f(0,-1,0); //up
    vec3f w = vec3f(0,0,1); //-viewing_dir
    float eps = 1e-3;
    float max_dist = 1e30;

    int samples_per_ray = 100;
    float focal_dist = 3;

    const char MODE = 'p'; // p = perspectivic; o = orthographic

    QRgb background_color = qRgb(255,255,255); //background color
    std::shared_ptr<shader> shdr;

    QRgb ray_color(ray r, float t0, float t1, int recursion_depth);
public:
    QImage render();
    view(int width, int height, vec3f viewer_pos, vec3f viewing_dir, mesh &msh, float viewing_dst, std::vector<vec3f> light_srcs, std::vector<float> light_intensites);
};

#endif // VIEW_H
