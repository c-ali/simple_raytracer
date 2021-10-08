#ifndef VIEW_H
#define VIEW_H

#include <vector>
#include <QImage>
#include <QColor>
#include "util.h"
#include "data_structures.h"

class view
{
private:
    int img_width, img_height;
    vec3d viewer_pos;
    mesh msh;
    float viewing_dst;
    std::vector<vec3d> light_srcs;
    bool shadows = true;
    std::vector<float> light_intensites;

    vec3d u = vec3d(1,0,0); //right
    vec3d v = vec3d(0,-1,0); //up
    vec3d w = vec3d(0,0,1); //-viewing_dir
    float eps = 0.01;
    float max_dist = 1e10;

    const char MODE = 'p'; // p = perspectivic; o = orthographic
public:
    QImage render();
    view(int width, int height, vec3d viewer_pos, mesh msh, float viewing_dst, std::vector<vec3d> light_srcs, std::vector<float> light_intensites);
};

#endif // VIEW_H
