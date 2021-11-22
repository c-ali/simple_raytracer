#include <QApplication>
#include <QMainWindow>
#include <QPainter>
#include <QLabel>
#include <memory>
#include "algebra.h"
#include "view.h"

QImage test_scene(){
    int width = 1000;
    int height = 1000;
    float viewing_dst = 2;
    //vec3d light_src = vec3d(3,8,-5);
    std::vector<vec3f> light_srcs;
    vec3f l1 = vec3f(10,0,10);
    vec3f l2 = vec3f(0,0,10);
    std::vector<float> light_intensities {0.3,0.5};
    vec3f viewer_pos = vec3f(0,0,5);

    mesh msh = mesh();
    std::shared_ptr<surface> sph = std::make_shared<sphere>(vec3f(2,0,0), 0.8);
    std::shared_ptr<surface> sph_ = std::make_shared<sphere>(vec3f(0,0,-3), 2);
    //std::shared_ptr<surface> tri = std::make_shared<triangle>(vec3d(2,0,0), vec3d(0,0,-3),vec3d(0,1,-10));
    msh.add_surface(sph);
    msh.add_surface(sph_);
    //msh.add_surface(tri);
    vec3f w = vec3f(0,0,1);


    light_srcs.push_back(l1);
    //light_srcs.push_back(l2);
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
    msh.add_surface(wall);
    msh.build_basic_tree(2, 50);
    view v = view(width,height, viewer_pos, w, msh, viewing_dst, light_srcs, light_intensities);
    QImage img = v.render();
    return img;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPixmap image;
    //QImage img = test_scene();
    QImage img = view_mesh();
    image = QPixmap::fromImage(img);
    //image.fill(Qt::black);

    QLabel lbl;
    lbl.setPixmap(image);
    lbl.show();

    return a.exec();
}

