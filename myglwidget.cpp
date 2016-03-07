#include "MyGLWidget.h"
#include <gl/glu.h>
#include <QtOpenGL>
#include <QKeyEvent>
#include <QTimer>


MyGLWidget::MyGLWidget(QWidget *parent) :
    QGLWidget(parent)
{
    rotX = 0.0f;
    rotY = 0.0f;
    rotZ = 0.0f;
    zoom = 1.0f;

}

MyGLWidget::~MyGLWidget()
{

}

void MyGLWidget::initializeGL()                         //此处开始对OpenGL进行所以设置
{
    glClearColor(0.0, 0.0, 0.0, 0.0);                   //白色背景
    glShadeModel(GL_SMOOTH);                            //启用阴影平滑

    glClearDepth(1.0);                                  //设置深度缓存
    glEnable(GL_DEPTH_TEST);                            //启用深度测试
    glDepthFunc(GL_LEQUAL);                             //所作深度测试的类型
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  //告诉系统对透视进行修正
}

void MyGLWidget::resizeGL(int w, int h)                 //重置OpenGL窗口的大小
{
    glViewport(0, 0, (GLint)w, (GLint)h);               //重置当前的视口
    glMatrixMode(GL_PROJECTION);                        //选择投影矩阵
    glLoadIdentity();                                   //重置投影矩阵
    //设置视口的大小
    gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);                         //选择模型观察矩阵
    glLoadIdentity();                                   //重置模型观察矩阵
}

void MyGLWidget::paintGL()                              //从这里开始进行所以的绘制
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清除屏幕和深度缓存
    glLoadIdentity();                                   //重置当前的模型观察矩阵

    draw();
}

void MyGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    GLfloat dx = (GLfloat)(event->x() - lastPos.x()) / width();
    GLfloat dy = (GLfloat)(event->y() - lastPos.y()) / height();
    if (event->buttons() & Qt::LeftButton)
    {
        rotX += 180 * dy;
        rotY += 180 * dx;
        updateGL();
    }
    else if (event->buttons() & Qt::RightButton)
    {
        rotX += 180 * dy;
        rotZ += 180 * dx;
        updateGL();
    }
    lastPos = event->pos();
}

void MyGLWidget::wheelEvent(QWheelEvent *event)
{
    GLfloat degrees = event->delta();
    zoom += degrees * 0.0002f;
    qDebug()<<"zoom = " << zoom;
    // 注意, 如果zoom为负值, eye就会被放置到z轴的负方向, 而从物体的背面看了.
    if (zoom < 1.0f) {
        zoom = 1.0f;
    }

    updateGL();
}

void MyGLWidget::draw()
{
    glTranslatef(0.0f, 0.0f, -5.0f);
    glLineWidth(1.0f);

    glScalef(zoom, zoom, zoom);

    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(rotZ, 0.0f, 0.0f, 1.0f);

    //x axis
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
        glVertex3f(-3.0f, 0.0f, 0.0f);
        glVertex3f(3.0f, 0.0f, 0.0f);
    glEnd();

    //y axis
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
        glVertex3f(0.0f, -3.0f, 0.0f);
        glVertex3f(0.0f, 3.0f, 0.0f);
    glEnd();

    //z axis
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, -3.0f);
        glVertex3f(0.0f, 0.0f, 3.0f);
    glEnd();

    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLES);
        glVertex3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();
}
