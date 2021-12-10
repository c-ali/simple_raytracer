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
    float viewing_dst = 7;
    //vec3d light_src = vec3d(3,8,-5);
    std::vector<vec3f> light_srcs;
    vec3f l1 = vec3f(10,0,10);
    vec3f l2 = vec3f(-10,0,10);
    std::vector<float> light_intensities {0.5,0.3};
    vec3f viewer_pos = vec3f(0,5,20);
    vec3f w = vec3f(0,0.2,1);
    light_srcs.push_back(l1);
    light_srcs.push_back(l2);
    mesh msh = mesh();
    msh.read_obj("bunny.obj");
    std::shared_ptr<surface> wall = std::make_shared<triangle>(vec3f(300,300,-3), vec3f(-300,300,-3), vec3f(0,-300,-3));
    wall->set_color(qRgb(255,255,255));
    //msh.add_surface(wall);
    msh.build_basic_tree(2, 50);
    view v = view(width,height, viewer_pos, w, msh, viewing_dst, light_srcs, light_intensities);
    QImage img = v.render();
    return img;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPixmap image;
    QImage img = focal_scene();
    //QImage img = view_mesh();
    //QImage img = mirror_scene();
    img.save("rendered.png");
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    image = QPixmap::fromImage(img);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    //image.fill(Qt::black);

    QLabel lbl;
    lbl.setPixmap(image);
    lbl.show();

    return a.exec();
}

