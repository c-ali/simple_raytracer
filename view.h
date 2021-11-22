#ifndef VIEW_H
#define VIEW_H

#include <vector>
#include <QImage>
#include <QColor>
#include "algebra.h"
#include "geometry.h"
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

    vec3f u = vec3f(1,0,0); //right
    vec3f v = vec3f(0,-1,0); //up
    vec3f w = vec3f(0,0,1); //-viewing_dir
    float eps = 0.01;
    float max_dist = 1e10;

    int samples_per_ray = 1;

    const char MODE = 'p'; // p = perspectivic; o = orthographic
public:
    QImage render();
    view(int width, int height, vec3f viewer_pos, vec3f viewing_dir, mesh &msh, float viewing_dst, std::vector<vec3f> light_srcs, std::vector<float> light_intensites);
};

#endif // VIEW_H
