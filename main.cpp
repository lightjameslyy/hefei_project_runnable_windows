#include "mainwindow.h"
#include "myglwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    MyGLWidget w1;
    w1.resize(1000,800);
    w1.show();

    return a.exec();
}
