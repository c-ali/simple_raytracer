#ifndef VIEW_H
#define VIEW_H

#include <vector>
#include <QImage>
#include <memory>
#include <QColor>
#include "algebra.h"
#include "geometry.h"
#include "shading.h"
#include <math.h>


extern int hit_count;


class view
{
public:
    //DOF options
    float focal_dist = -1; //Use -1 to turn off DOF
    float aperture = 0.01;

    //distributed raytracing options
    int samples_per_ray = 1;
    bool anti_alias = false;

    //path tracing options
    bool path_tracing = false;
    float lighting_fac = 300;
    float roulette_prob = 1;
    bool next_event = false;

    //multithreading options
    int num_threads = 15;
    bool shadows = true;

    //general raytracing options
    int max_recursion_depth = 5;
private:
    QImage img;
    int img_width, img_height;
    vec3f viewer_pos;
    mesh msh;
    float viewing_dst;
    std::vector<vec3f> light_srcs;
    std::vector<float> light_intensites;

    const vec3f u = vec3f(1,0,0); //right
    const vec3f v = vec3f(0,-1,0); //up
    vec3f w = vec3f(0,0,1); //-viewing_dir
    const float eps = 1e-3;
    const float max_dist = 1e30;

    const char MODE = 'p'; // p = perspectivic; o = orthographic

    vec3f background_color = vec3f(0,0,0); //background color
    std::shared_ptr<shader> shdr;

    const float p_diffuse =  1 / (2 * M_PI);
    vec3f ray_color(ray r, float t0, float t1, int recursion_depth);
    vec3f trace_color(ray r, int recursion_depth);
    void compute_lines(int j_start, int j_end);
public:
    QImage render();
    view(int width, int height, vec3f viewer_pos, vec3f viewing_dir, mesh &msh, float viewing_dst, std::vector<vec3f> light_srcs, std::vector<float> light_intensites);
};


ray random_ray_in_hemisphere_reject(const vec3f &origin, const vec3f &normal);
ray random_ray_in_hemisphere_constr(const vec3f &origin, const vec3f &normal);
float randf(float scale);


#endif // VIEW_H
