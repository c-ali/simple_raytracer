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
    float radius = 1;
    vec3d center = vec3d(0,0,-5);


    mesh msh = mesh();
    vec3d viewer_pos = vec3d(0,0,0);
    std::shared_ptr<surface> sph = std::make_shared<sphere>(center, radius);
    msh.add_surface(sph);
    view v = view(width,height, viewer_pos, msh, viewing_dst);
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

