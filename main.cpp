#include <QApplication>
#include <QMainWindow>
#include <QPainter>
#include <QLabel>
#include <memory>
#include <cmath>
#include <chrono>
#include <iostream>
#include "algebra.h"
#include "view.h"

QImage aligned_reflective_gallery(){
    int width = 1000;
    int height = 1000;
    float viewing_dst = 2;

    std::vector<vec3f> light_srcs;
    vec3f l1 = vec3f(10,10,10);
    vec3f l2 = vec3f(-10,10,10);
    std::vector<float> light_intensities {0.3,0.5};
    light_srcs.push_back(l1);
    light_srcs.push_back(l2);
    vec3f viewer_pos = vec3f(0,5,8);
    vec3f w = vec3f(0,0.5,2);

    mesh msh = mesh();
    std::shared_ptr<checkerboard> floor = std::make_shared<checkerboard>(0, false);
    msh.add_surface(floor);

    // Parameters for Pyramids
    float pyramid_base_width = 2.0f;
    float pyramid_height = 6.0f;
    float pyramid_depth = -12.0f; // Z-coordinate for the pyramid base
    float pyramid_rotation = M_PI / 4; // Rotation angle in radians

    // Rotation matrix for the pyramid
    float cos_theta = cos(pyramid_rotation);
    float sin_theta = sin(pyramid_rotation);

    auto rotate_point = [cos_theta, sin_theta](vec3f point, vec3f origin) {
        float x_new = cos_theta * (point.x - origin.x) - sin_theta * (point.z - origin.z) + origin.x;
        float z_new = sin_theta * (point.x - origin.x) + cos_theta * (point.z - origin.z) + origin.z;
        return vec3f(x_new, point.y, z_new);
    };

    vec3f origin = vec3f(0, 0, pyramid_depth);

    // Reflective Spheres Around the Pyramid
    int num_spheres = 5;
    float radius = 8.0f; // Radius of the circle around the pyramid
    for (int i = 0; i < num_spheres; ++i) {
        float angle = 2 * M_PI * i / num_spheres;
        float x = radius * cos(angle);
        float z = radius * sin(angle) + pyramid_depth;
        std::shared_ptr<sphere> sph = std::make_shared<sphere>(vec3f(x, pyramid_height/2, z), 1.5);
        sph->specular = true;
        sph->set_color(qRgb(255, 255, 255)); // Reflective spheres
        msh.add_surface(sph);
    }

    // Upright Pyramid (with rotation and different colors)
    std::shared_ptr<surface> tri1 = std::make_shared<triangle>(
        rotate_point(vec3f(0, pyramid_height, pyramid_depth), origin),
        rotate_point(vec3f(-pyramid_base_width / 2, 0, pyramid_depth - pyramid_base_width / 2), origin),
        rotate_point(vec3f(pyramid_base_width / 2, 0, pyramid_depth - pyramid_base_width / 2), origin)
        );
    tri1->set_color(qRgb(255, 105, 180)); // Pink

    std::shared_ptr<surface> tri2 = std::make_shared<triangle>(
        rotate_point(vec3f(0, pyramid_height, pyramid_depth), origin),
        rotate_point(vec3f(pyramid_base_width / 2, 0, pyramid_depth - pyramid_base_width / 2), origin),
        rotate_point(vec3f(pyramid_base_width / 2, 0, pyramid_depth + pyramid_base_width / 2), origin)
        );
    tri2->set_color(qRgb(135, 206, 235)); // Sky Blue

    std::shared_ptr<surface> tri3 = std::make_shared<triangle>(
        rotate_point(vec3f(0, pyramid_height, pyramid_depth), origin),
        rotate_point(vec3f(pyramid_base_width / 2, 0, pyramid_depth + pyramid_base_width / 2), origin),
        rotate_point(vec3f(-pyramid_base_width / 2, 0, pyramid_depth + pyramid_base_width / 2), origin)
        );
    tri3->set_color(qRgb(255, 105, 180)); // Pink

    std::shared_ptr<surface> tri4 = std::make_shared<triangle>(
        rotate_point(vec3f(0, pyramid_height, pyramid_depth), origin),
        rotate_point(vec3f(-pyramid_base_width / 2, 0, pyramid_depth + pyramid_base_width / 2), origin),
        rotate_point(vec3f(-pyramid_base_width / 2, 0, pyramid_depth - pyramid_base_width / 2), origin)
        );
    tri4->set_color(qRgb(135, 206, 235)); // Sky Blue

    msh.add_surface(tri1);
    msh.add_surface(tri2);
    msh.add_surface(tri3);
    msh.add_surface(tri4);

    // Inverted Pyramid (on top of the upright pyramid)
    vec3f top_origin = vec3f(0, pyramid_height, pyramid_depth);

    std::shared_ptr<surface> tri5 = std::make_shared<triangle>(
        rotate_point(vec3f(0, 0, pyramid_depth), top_origin),
        rotate_point(vec3f(-pyramid_base_width / 2, pyramid_height, pyramid_depth - pyramid_base_width / 2), top_origin),
        rotate_point(vec3f(pyramid_base_width / 2, pyramid_height, pyramid_depth - pyramid_base_width / 2), top_origin)
        );
    tri5->set_color(qRgb(255, 105, 180)); // Pink

    std::shared_ptr<surface> tri6 = std::make_shared<triangle>(
        rotate_point(vec3f(0, 0, pyramid_depth), top_origin),
        rotate_point(vec3f(pyramid_base_width / 2, pyramid_height, pyramid_depth - pyramid_base_width / 2), top_origin),
        rotate_point(vec3f(pyramid_base_width / 2, pyramid_height, pyramid_depth + pyramid_base_width / 2), top_origin)
        );
    tri6->set_color(qRgb(135, 206, 235)); // Sky Blue

    std::shared_ptr<surface> tri7 = std::make_shared<triangle>(
        rotate_point(vec3f(0, 0, pyramid_depth), top_origin),
        rotate_point(vec3f(pyramid_base_width / 2, pyramid_height, pyramid_depth + pyramid_base_width / 2), top_origin),
        rotate_point(vec3f(-pyramid_base_width / 2, pyramid_height, pyramid_depth + pyramid_base_width / 2), top_origin)
        );
    tri7->set_color(qRgb(255, 105, 180)); // Pink

    std::shared_ptr<surface> tri8 = std::make_shared<triangle>(
        rotate_point(vec3f(0, 0, pyramid_depth), top_origin),
        rotate_point(vec3f(-pyramid_base_width / 2, pyramid_height, pyramid_depth + pyramid_base_width / 2), top_origin),
        rotate_point(vec3f(-pyramid_base_width / 2, pyramid_height, pyramid_depth - pyramid_base_width / 2), top_origin)
        );
    tri8->set_color(qRgb(135, 206, 235)); // Sky Blue

    msh.add_surface(tri5);
    msh.add_surface(tri6);
    msh.add_surface(tri7);
    msh.add_surface(tri8);

    view v = view(width, height, viewer_pos, w, msh, viewing_dst, light_srcs, light_intensities);
    v.samples_per_ray = 50;
    v.aperture = 0.05;
    v.focal_dist = 10;
    v.shadows = true;
    v.path_tracing = false; // Path tracing turned off
    v.cos_weighted = true;
    v.num_threads = 10;
    v.next_event = true;
    v.anti_alias = true;

    QImage img = v.render();
    return img;
}


QImage focal_scene(){
    int width = 1000;
    int height = 1000;
    float viewing_dst = 1;
    //vec3d light_src = vec3d(3,8,-5);
    std::vector<vec3f> light_srcs;
    vec3f l1 = vec3f(10,0,10);
    vec3f l2 = vec3f(0,0,10);
    std::vector<float> light_intensities {0.3,0.5};
    vec3f viewer_pos = vec3f(0,0,3);

    mesh msh = mesh();

    for(int i = 0; i < 8; ++i){
        std::shared_ptr<surface> sph = std::make_shared<sphere>(vec3f(1.4*i-2,0,-i), 0.5);
        msh.add_surface(sph);
    }
    msh.build_basic_tree(2,10);
    //msh.add_surface(tri);
    vec3f w = vec3f(0,0,1);

    light_srcs.push_back(l1);
    //light_srcs.push_back(l2);
    view v = view(width,height, viewer_pos,w, msh, viewing_dst, light_srcs, light_intensities);
    v.focal_dist = std::sqrt(13)-0.5;
    v.aperture = 0.1;
    v.samples_per_ray = 20;
    QImage img = v.render();
    return img;
}

QImage mirror_scene(){
    int width = 1000;
    int height = 1000;
    float viewing_dst = 2;
    //vec3d light_src = vec3d(3,8,-5);
    std::vector<vec3f> light_srcs;
    vec3f l1 = vec3f(10,10,10);
    vec3f l2 = vec3f(-10,10,10);
    std::vector<float> light_intensities {0.3,0.5};
    bool refract = true; // this sets the scene up to showcase refraction instead of reflection

    mesh msh = mesh();
    std::shared_ptr<surface> sph1 = std::make_shared<sphere>(vec3f(2,0.8,0), 0.8);
    std::shared_ptr<surface> sph3 = std::make_shared<sphere>(vec3f(0,1,0), 1);
    if(refract){
        sph1 = std::make_shared<sphere>(vec3f(6,4,-30), 2);
        sph3 = std::make_shared<sphere>(vec3f(0,4,-30), 3);
    }
    std::shared_ptr<surface> sph2 = std::make_shared<sphere>(vec3f(0,2,-5), 2);
    std::shared_ptr<checkerboard> floor = std::make_shared<checkerboard>(0, false);
    float h = 0.;
    std::shared_ptr<surface> tri = std::make_shared<triangle>(vec3f(-100,h,100), vec3f(100,h,100),vec3f(0,h,-100));
    tri->color = qRgb(200,200,200);

    if(refract){
        sph2->specular = false;
        sph2->refract_eta = 10;
    }
    else
        sph2->specular = true;
    sph1->color = qRgb(255,0,0);
    sph3->color = qRgb(0,0,255);
    floor->color = qRgb(105,150,150);

    msh.add_surface(sph1);
    msh.add_surface(sph2);
    msh.add_surface(sph3);
    //msh.add_surface(tri);
    msh.add_surface(floor);

    vec3f viewer_pos = vec3f(0,5,8);
    vec3f w = vec3f(0,0.5,2);

    light_srcs.push_back(l1);
    light_srcs.push_back(l2);
    view v = view(width,height, viewer_pos,w, msh, viewing_dst, light_srcs, light_intensities);
    v.anti_alias = true;
    v.cos_weighted = true;
    v.shadows = false;
    v.samples_per_ray = 100;
    QImage img = v.render();

    return img;
}

QImage view_mesh(){
    int width = 1000;
    int height = 1000;
    float viewing_dst = 0.68;
    //vec3d light_src = vec3d(3,8,-5);
    std::vector<vec3f> light_srcs;
    vec3f l1 = vec3f(10,100,100);
    vec3f l2 = vec3f(-10,100,100);
    std::vector<float> light_intensities {0.6,0.8};
    vec3f viewer_pos = vec3f(30,-10,115);
    vec3f w = vec3f(0,0.25,1);
    light_srcs.push_back(l1);
    //light_srcs.push_back(l2);
    mesh msh = mesh();
    msh.read_obj("/Users/cali/workspace/simple_raytracer/xyzrgb_dragon.obj");
    std::shared_ptr<surface> floor1 = std::make_shared<triangle>(vec3f(11111,11111,-500), vec3f(-11111,11111,-500), vec3f(0,-11111,-500));
    floor1->set_color(qRgb(211,211,211));
    msh.add_surface(floor1);
    msh.build_fast_tree(2, 50);
    view v = view(width,height, viewer_pos, w, msh, viewing_dst, light_srcs, light_intensities);
    v.focal_dist = 120;
    QImage img = v.render();
    return img;
}

QImage cornell_box(){
    int width = 500;
    int height = 500;
    float viewing_dst = 0.38;
    float intensity = 200;
    vec3f w = vec3f(0,0,1);
    mesh msh = mesh();
    vec3f viewer_pos = vec3f(0,5,-0.1);
    QRgb red = qRgb(intensity,0,0);
    QRgb green = qRgb(0,intensity,0);
    QRgb blue = qRgb(0,0,intensity);
    QRgb white = qRgb(intensity,intensity,intensity);
    QRgb black = qRgb(0,0,0);

    //left wall
    msh.add_surface(std::make_shared<triangle>(vec3f(-10,0,0), vec3f(-10,10,0), vec3f(-10,0,-10), vec3f(1,0,0), red));
    msh.add_surface(std::make_shared<triangle>(vec3f(-10,0,-10), vec3f(-10,10,0), vec3f(-10,10,-10), vec3f(1,0,0), red));

    //right wall
    msh.add_surface(std::make_shared<triangle>(vec3f(10,10,0), vec3f(10,0,0), vec3f(10,0,-10), vec3f(-1,0,0), blue));
    msh.add_surface(std::make_shared<triangle>(vec3f(10,10,0), vec3f(10,0,-10), vec3f(10,10,-10), vec3f(-1,0,0), blue));


    //back wall
    msh.add_surface(std::make_shared<triangle>(vec3f(-10,0,-10),vec3f(10,10,-10), vec3f(-10,10,-10), vec3f(0,0,1), white));
    msh.add_surface(std::make_shared<triangle>(vec3f(10,10,-10), vec3f(-10,0,-10), vec3f(10,0,-10), vec3f(0,0,1), white));

    //front wall
    msh.add_surface(std::make_shared<triangle>(vec3f(-10,0,0),vec3f(10,10,0), vec3f(-10,10,0),vec3f(0,0,-1), black));
    msh.add_surface(std::make_shared<triangle>(vec3f(10,10,0), vec3f(-10,0,0), vec3f(10,0,0), vec3f(0,0,-1), black));

    //ceiling
    msh.add_surface(std::make_shared<triangle>(vec3f(10,10,0), vec3f(-10,10,0), vec3f(10,10,-10), vec3f(0,-1,0), white));
    msh.add_surface(std::make_shared<triangle>(vec3f(10,10,-10), vec3f(-10,10,0), vec3f(-10,10,-10), vec3f(0,-1,0), white));

    //floor
    msh.add_surface(std::make_shared<triangle>(vec3f(-10,0,0), vec3f(10,0,0), vec3f(10,0,-10),vec3f(0,1,0), white));
    msh.add_surface(std::make_shared<triangle>(vec3f(-10,0,0), vec3f(10,0,-10), vec3f(-10,0,-10),vec3f(0,1,0), white));

    //add spheres
    std::shared_ptr<surface> sph = std::make_shared<sphere>(vec3f(5,2,-5), 3, white);
    sph->specular = true;
//    msh.add_surface(sph);
    //msh.add_surface(std::make_shared<sphere>(vec3f(-6,1,-8), 2, white));

    //add light
    msh.add_surface(std::make_shared<sphere>(vec3f(0,10,-5), 2, white, white));

    std::vector<vec3f> lamps;
    lamps.push_back(vec3f(0,3,-5));
    std::vector<float> intensities;
    intensities.push_back(1);
    view v = view(width, height, viewer_pos, w, msh, viewing_dst, lamps, intensities);
    v.samples_per_ray = 500;
    v.path_tracing = true;
    v.lighting_fac = 5000;
    v.shadows = false;
    v.num_threads = 1;
    v.next_event = true;
    v.cos_weighted = true;
    msh.build_basic_tree(2, 50);
    return v.render();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPixmap image;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    //QImage img = view_mesh();
    //QImage img = focal_scene();
    //QImage img = cornell_box();
    QImage img = mirror_scene();
    //QImage img = aligned_reflective_gallery();
    img.save("../../../../../rendered.png");
    image = QPixmap::fromImage(img);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
    //image.fill(Qt::black);

    QLabel lbl;
    lbl.setPixmap(image);
    lbl.show();

    return a.exec();
}

