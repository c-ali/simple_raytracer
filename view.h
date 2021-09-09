#ifndef VIEW_H
#define VIEW_H

#include <vector>
#include <QImage>
#include <QColor>
#include "util.h"


class view
{
private:
    vec3d viewer_pos, light_src;
    float viewing_dst;
    vec3d u = vec3d(1,0,0); //right
    vec3d v = vec3d(0,-1,0); //up
    vec3d w = vec3d(0,0,1); //-viewing_dir

    float min_dist = 1e-10;
    float max_dist = 1e10;

    mesh msh;
    int img_width, img_height;
    const char MODE = 'p'; // p = perspectivic; o = orthographic
public:
    QImage render();
    view(int width, int height, vec3d viewer_pos, mesh msh, float viewing_dst, vec3d light_src);
};

#endif // VIEW_H
