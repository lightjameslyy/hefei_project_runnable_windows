#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QWidget>
#include <QGLWidget>

class MyGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MyGLWidget(QWidget *parent = 0);
    MyGLWidget(QString dataDir);
    ~MyGLWidget();


protected:
    //对3个纯虚函数的重定义
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    GLfloat rotX;                                   //绕x轴旋转的角度
    GLfloat rotY;                                   //绕y轴旋转的角度
    GLfloat rotZ;                                   //绕z轴旋转的角度
    GLfloat moveX;                                  //沿x轴平移的距离
    GLfloat moveY;                                  //沿y轴平移的距离
    GLfloat moveZ;                                  //沿z轴平移的距离
    QPoint lastPos;                                 //上次点击位置
    GLfloat zoom;                                   //缩放倍数

    QString dataDir = "";

    void drawAxis();
    void draw();
//    bool fullscreen;                                //是否全屏显示
};

#endif // MYGLWIDGET_H