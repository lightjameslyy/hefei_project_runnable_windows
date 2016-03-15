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
    moveX = 0.0f;
    moveY = 0.0f;
    moveZ = 0.0f;
    zoom = 1.0f;

}

MyGLWidget::MyGLWidget(QString dataDir)
{
    rotX = 0.0f;
    rotY = 0.0f;
    rotZ = 0.0f;
    moveX = 0.0f;
    moveY = 0.0f;
    moveZ = 0.0f;
    zoom = 1.0f;
    this->dataDir = dataDir;
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

    glTranslatef(0.0f, 0.0f, -50.0f);

    glTranslatef(moveX, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, moveZ);

    glScalef(zoom, zoom, zoom);

    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(rotZ, 0.0f, 0.0f, 1.0f);

    drawAxis();

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
    GLfloat mx = (GLfloat)(event->x() - lastPos.x());
    GLfloat mz = (GLfloat)(event->y() - lastPos.y());
    if (event->buttons() & Qt::LeftButton)
    {
        moveX += mx*0.02;
        moveZ += mz*0.02;
        updateGL();
//        rotX += 180 * dy;
//        rotY += 180 * dx;
//        updateGL();
    }
    else if (event->buttons() & Qt::RightButton)
    {
        rotX += 180 * dy;
        rotY += 180 * dx;
        updateGL();
    }
    lastPos = event->pos();
}

void MyGLWidget::wheelEvent(QWheelEvent *event)
{
    GLfloat degrees = event->delta();
    zoom += degrees * 0.0002f;
    qDebug()<<"zoom = " << zoom;
    //最小缩小10倍
    if (zoom < 0.1f) {
        zoom = 0.1f;
    }
    updateGL();
}

void MyGLWidget::drawAxis()
{
    glLineWidth(0.5f);
    for (GLfloat i = 0.0f; i <= 36; i += 0.3f)
    {
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_LINES);
            glVertex3f(-18.0f, 0.0f, -18.0f+i);
            glVertex3f(18.0f, 0.0f, -18.0f+i);
        glEnd();
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_LINES);
            glVertex3f(-18.0f+i, 0.0f, -18.0f);
            glVertex3f(-18.0f+i, 0.0f, 18.0f);
        glEnd();
    }

    glLineWidth(2.0f);
    //x axis
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
        glVertex3f(-18.0f, 0.0f, 0.0f);
        glVertex3f(18.0f, 0.0f, 0.0f);
        glVertex3f(18.0f, 0.0f, 0.0f);
        glVertex3f(17.5f, 0.0f, 0.3f);
        glVertex3f(18.0f, 0.0f, 0.0f);
        glVertex3f(17.5f, 0.0f, -0.3f);
    glEnd();

    //y axis
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
        glVertex3f(0.0f, -18.0f, 0.0f);
        glVertex3f(0.0f, 18.0f, 0.0f);
    glEnd();

    //z axis
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, -18.0f);
        glVertex3f(0.0f, 0.0f, 18.0f);
        glVertex3f(0.0f, 0.0f, -18.0f);
        glVertex3f(0.3f, 0.0f, -17.5f);
        glVertex3f(0.0f, 0.0f, -18.0f);
        glVertex3f(-0.3f, 0.0f, -17.5f);
    glEnd();

//    glLineWidth(20.0f);
//    glBegin(GL_LINES);
//        glColor3f(1.0f, 0.0f, 0.0f);
//        glVertex3f(1.0f, 0.0f, 1.0f);
//        glColor3f(0.0f, 1.0f, 0.0f);
//        glVertex3f(1.0f, 1.0f, 1.0f);
//    glEnd();

//    glColor3f(1.0f, 1.0f, 0.0f);
//    glBegin(GL_TRIANGLES);
//        glVertex3f(1.0f, 0.0f, 0.0f);
//        glVertex3f(0.0f, 1.0f, 0.0f);
//        glVertex3f(0.0f, 0.0f, 1.0f);
    //    glEnd();
}

void MyGLWidget::draw()
{
    //根据dir得到.pmpl文件列表
    QDir dir(dataDir);
    QFileInfoList infoList = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);


    int xPixels = infoList.size();
    qDebug() << xPixels;
}
