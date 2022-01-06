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

    mesh msh = mesh();
    std::shared_ptr<surface> sph1 = std::make_shared<sphere>(vec3f(2,0.8,0), 0.8);
    std::shared_ptr<surface> sph2 = std::make_shared<sphere>(vec3f(0,2,-5), 2);
    std::shared_ptr<surface> sph3 = std::make_shared<sphere>(vec3f(0,1,0), 1);
    std::shared_ptr<checkerboard> floor = std::make_shared<checkerboard>(0);
    float h = 0.;
    std::shared_ptr<surface> tri = std::make_shared<triangle>(vec3f(-100,h,100), vec3f(100,h,100),vec3f(0,h,-100));
    tri->color = qRgb(200,200,200);

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
    QImage img = v.render();
    return img;
}

QImage view_mesh(){
    int width = 500;
    int height = 500;
    float viewing_dst = 0.68;
    //vec3d light_src = vec3d(3,8,-5);
    std::vector<vec3f> light_srcs;
    vec3f l1 = vec3f(10,100,100);
    vec3f l2 = vec3f(-10,100,100);
    std::vector<float> light_intensities {0.6,0.8};
    vec3f viewer_pos = vec3f(0,100,120);
    vec3f w = vec3f(0,0.2,1);
    light_srcs.push_back(l1);
    //light_srcs.push_back(l2);
    mesh msh = mesh();
    msh.read_obj("treehq_tri.obj");
    std::shared_ptr<surface> floor1 = std::make_shared<triangle>(vec3f(11111,11111,-500), vec3f(-11111,11111,-500), vec3f(0,-11111,-500));
    std::shared_ptr<surface> sph1 = std::make_shared<sphere>(vec3f(100,100,-50), 20);
    sph1->set_color(qRgb(0,0,100));
    std::shared_ptr<surface> sph2 = std::make_shared<sphere>(vec3f(-100,100,-100), 20);
    sph2->set_color(qRgb(0,0,100));
    floor1->set_color(qRgb(100,0,0));

    msh.add_surface(floor1);
    msh.add_surface(sph1);
    msh.add_surface(sph2);
    msh.build_basic_tree(2, 50);
    view v = view(width,height, viewer_pos, w, msh, viewing_dst, light_srcs, light_intensities);
    v.focal_dist = 120;
    QImage img = v.render();
    return img;
}

QImage cornell_box(){
    int width = 500;
    int height = 500;
    float viewing_dst = 0.38;
    vec3f w = vec3f(0,0,1);
    mesh msh = mesh();
    vec3f viewer_pos = vec3f(0,5,-0.1);
    QRgb red = qRgb(255,0,0);
    QRgb green = qRgb(0,255,0);
    QRgb blue = qRgb(0,0,255);
    QRgb white = qRgb(255,255,255);
    QRgb grey = qRgb(100,100,100);

    //left wall
    msh.add_surface(std::make_shared<triangle>(vec3f(-10,0,0), vec3f(-10,10,0), vec3f(-10,0,-10), vec3f(1,0,0), red));
    msh.add_surface(std::make_shared<triangle>(vec3f(-10,0,-10), vec3f(-10,10,0), vec3f(-10,10,-10), vec3f(1,0,0), red));

    //right wall
    msh.add_surface(std::make_shared<triangle>(vec3f(10,10,0), vec3f(10,0,0), vec3f(10,0,-10), vec3f(-1,0,0), blue));
    msh.add_surface(std::make_shared<triangle>(vec3f(10,10,0), vec3f(10,0,-10), vec3f(10,10,-10), vec3f(-1,0,0), blue));


    //back wall
    msh.add_surface(std::make_shared<triangle>(vec3f(-10,0,-10),vec3f(10,10,-10), vec3f(-10,10,-10), vec3f(0,0,1), grey));
    msh.add_surface(std::make_shared<triangle>(vec3f(10,10,-10), vec3f(-10,0,-10), vec3f(10,0,-10), vec3f(0,0,1), grey));

    //front wall
    msh.add_surface(std::make_shared<triangle>(vec3f(-10,0,0),vec3f(10,10,0), vec3f(-10,10,0),vec3f(0,0,-1), white));
    msh.add_surface(std::make_shared<triangle>(vec3f(10,10,0), vec3f(-10,0,0), vec3f(10,0,0), vec3f(0,0,-1), white));

    //ceiling
    msh.add_surface(std::make_shared<triangle>(vec3f(10,10,0), vec3f(-10,10,0), vec3f(10,10,-10), vec3f(0,-1,0), white));
    msh.add_surface(std::make_shared<triangle>(vec3f(10,10,-10), vec3f(-10,10,0), vec3f(-10,10,-10), vec3f(0,-1,0), white));

    //floor
    msh.add_surface(std::make_shared<triangle>(vec3f(-10,0,0), vec3f(10,0,0), vec3f(10,0,-10),vec3f(0,1,0), white));
    msh.add_surface(std::make_shared<triangle>(vec3f(-10,0,0), vec3f(10,0,-10), vec3f(-10,0,-10),vec3f(0,1,0), white));

    //add spheres
    //msh.add_surface(std::make_shared<sphere>(vec3f(6,1,-8), 2, white));
    //msh.add_surface(std::make_shared<sphere>(vec3f(-6,1,-8), 2, white));

    //add light
    msh.add_surface(std::make_shared<sphere>(vec3f(0,10,-7), 3, white, white));

    std::vector<vec3f> lamps;
    lamps.push_back(vec3f(0,5,-5));
    std::vector<float> intensities;
    intensities.push_back(1);
    view v = view(width,height, viewer_pos, w, msh, viewing_dst, lamps, intensities);
    v.samples_per_ray = 10;
    v.max_recursion_depth = 5;
    v.path_tracing = false;
    v.shadows = false;
    return v.render();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPixmap image;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    //QImage img = focal_scene();
    QImage img = cornell_box();
    //QImage img = mirror_scene();
    img.save("rendered.png");
    image = QPixmap::fromImage(img);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    //image.fill(Qt::black);

    QLabel lbl;
    lbl.setPixmap(image);
    lbl.show();

    return a.exec();
}

