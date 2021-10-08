#include <QApplication>
#include <QMainWindow>
#include <QPainter>
#include <QLabel>
#include <memory>
#include "util.h"
#include "view.h"

QImage test_scene(){
    int width = 500;
    int height = 500;
    float viewing_dst = 2;
    //vec3d light_src = vec3d(3,8,-5);
    std::vector<vec3d> light_srcs;
    vec3d l1 = vec3d(10,0,10);
    vec3d l2 = vec3d(0,0,10);
    std::vector<float> light_intensities {2,2};

    mesh msh = mesh();
    vec3d viewer_pos = vec3d(0,0,5);
    std::shared_ptr<surface> sph = std::make_shared<sphere>(vec3d(2,0,0), 0.3);
    std::shared_ptr<surface> sph_ = std::make_shared<sphere>(vec3d(0,0,-3), 2);
    msh.add_surface(sph);
    msh.add_surface(sph_);
    light_srcs.push_back(l1);
    //light_srcs.push_back(l2);
    view v = view(width,height, viewer_pos, msh, viewing_dst, light_srcs, light_intensities);
    QImage img = v.render();
    return img;
}

QImage view_mesh(){
    int width = 500;
    int height = 500;
    float viewing_dst = 2;
    //vec3d light_src = vec3d(3,8,-5);
    std::vector<vec3d> light_srcs;
    vec3d l1 = vec3d(10,0,10);
    vec3d l2 = vec3d(0,0,10);
    std::vector<float> light_intensities {2,2};

    mesh msh = mesh();
    msh.read_obj("cube.obj");
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

