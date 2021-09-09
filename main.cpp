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
    vec3d light_src = vec3d(0,0,1);

    mesh msh = mesh();
    vec3d viewer_pos = vec3d(0,0,0);
    std::shared_ptr<surface> sph = std::make_shared<sphere>(vec3d(0,0,-1), 0.3);
    std::shared_ptr<surface> sph_ = std::make_shared<sphere>(vec3d(0,0,-2), 0.7);
    msh.add_surface(sph);
    msh.add_surface(sph_);
    view v = view(width,height, viewer_pos, msh, viewing_dst, light_src);
    QImage img = v.render();
    return img;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPixmap image;
    QImage img = test_scene();
    image = QPixmap::fromImage(img);
    //image.fill(Qt::black);

    QLabel lbl;
    lbl.setPixmap(image);
    lbl.show();

    return a.exec();
}
