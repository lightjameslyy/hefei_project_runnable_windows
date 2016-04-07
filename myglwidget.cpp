#include "MyGLWidget.h"
#include <cstdio>
#include <algorithm>
#include <gl/glu.h>
#include <QtOpenGL>
#include <QKeyEvent>
#include <QTimer>
#include <QMap>


MyGLWidget::MyGLWidget(QWidget *parent) :
    QGLWidget(parent)
{
    rotX = 20.0f;
    rotY = -20.0f;
    rotZ = 0.0f;
    moveX = 0.0f;
    moveY = 0.0f;
    moveZ = 0.0f;
    zoom = 1.0f;

}

MyGLWidget::MyGLWidget(QString dataDir)
{
    rotX = 20.0f;
    rotY = -20.0f;
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
    glClearColor(1.0, 1.0, 1.0, 0.0);                   //白色背景
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
    gluPerspective(60.0, (GLfloat)w/(GLfloat)h, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);                         //选择模型观察矩阵
    glLoadIdentity();                                   //重置模型观察矩阵
}

void MyGLWidget::paintGL()                              //从这里开始进行所以的绘制
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清除屏幕和深度缓存
    glLoadIdentity();                                   //重置当前的模型观察矩阵

    glTranslatef(0.0f, -15.0f, -50.0f);

    glTranslatef(moveX, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, moveZ);

    glScalef(zoom, zoom, zoom);

    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(rotZ, 0.0f, 0.0f, 1.0f);

//    qDebug()<<rotX<<rotY<<rotZ;
    drawAxis();

    draw3D();
}

bool MyGLWidget::get3DDataParam(QFileInfoList list)
{
    this->data3D.clear();
    int fileNum = list.size();

//    QString lastPosInfo = "";
    MapPos* lastPos = new MapPos(0.0, 0.0);
    QMap<QString, int> mapPosToFileIndex;
    int wrongFileCount = 0;
    for (int i = 0; i < fileNum; i++)      //遍历所有.pmpl文件
    {
        QFile file(list[i].absoluteFilePath());
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug()<<"文件未找到";
            return false;
        }
        QTextStream in(&file);
        QString posInfo;
        in>>posInfo;
        MapPos* pos = getMapPosFromLine(posInfo);
        if (!pos) {
            ++wrongFileCount;
            continue;       //格式不符
        }
        mapPosToFileIndex[posInfo] = i - wrongFileCount;

//        if (lastPosInfo == posInfo )       //忽略重复的的地理坐标
//            continue;
//        else {
//            minLongtitude = std::min(minLongtitude, pos->longtitude);
//            minLatitude = std::min(minLatitude, pos->latitude);
//            lastPosInfo = posInfo;
//        }
        in.readLine();
        in.readLine();

        QVector<MetaDataFormat*> metaDatas;
        setMaxHeight(0);
        setMaxValue(0);
        int height;
        double value;
        while (in.atEnd() == false) {
            in>>height>>value;
            metaDatas.push_back(new MetaDataFormat(height, value));
            setMaxHeight(std::max(getMaxHeight(), height));
            setMaxValue(std::max(getMaxValue(), value));
            in.readLine();
        }
        this->data3D.push_back(new MapDataFormat(pos, metaDatas));

    }
    uniquePosIndex = mapPosToFileIndex.values();
    qSort(uniquePosIndex.begin(), uniquePosIndex.end());
//    for (QMap<QString, int>::iterator i = mapPosToFileIndex.begin(); i != mapPosToFileIndex.end(); ++i) {
//        qDebug()<<i.key()<<i.value();
//    }
//    for (auto i : uniquePosIndex)
//        qDebug()<<i;
    double firstLot = data3D[0]->pos->longtitude*100;
    double firstLat = data3D[0]->pos->latitude*100;
    data3D[0]->pos->longtitude = 0.0;
    data3D[0]->pos->latitude = 0.0;
//    qDebug()<<data3D[0]->pos->longtitude<<data3D[0]->pos->latitude;
    for (auto i : uniquePosIndex) {
        data3D[i]->pos->longtitude = data3D[i]->pos->longtitude*100 - firstLot;
        data3D[i]->pos->latitude = data3D[i]->pos->latitude*100 - firstLat;
//        qDebug()<<i<<data3D[i]->pos->longtitude<<data3D[i]->pos->latitude;
    }
//    for (int i = 0; i < data3D.size(); i += 10) {
//        qDebug()<<data3D[i]->pos->longtitude<<data3D[i]->pos->latitude;
//    }
//    qDebug()<<maxHeight<<maxValue;
//    qDebug()<<data3D.size();
    return true;
}

MapPos* MyGLWidget::getMapPosFromLine(QString s)
{
    if (s.indexOf(',') ==  -1) return NULL;
    double longtitude;
    double latitude;
    int colonPos = s.indexOf(',');
    QString lot = s.left(colonPos - 1);
    QString lat = s.mid(colonPos+1, s.size()-colonPos-2);
    longtitude = lot.toDouble();
    latitude = lat.toDouble();
    return new MapPos(longtitude, latitude);
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

double MyGLWidget::getMaxValue() const
{
    return maxValue;
}

void MyGLWidget::setMaxValue(double value)
{
    maxValue = value;
}

int MyGLWidget::getMaxHeight() const
{
    return maxHeight;
}

void MyGLWidget::setMaxHeight(int value)
{
    maxHeight = value;
}

void MyGLWidget::drawAxis()
{
//    glLineWidth(0.5f);
//    for (GLfloat i = 0.0f; i <= 36; i += 0.3f)
//    {
//        glColor3f(1.0f, 1.0f, 1.0f);
//        glBegin(GL_LINES);
//            glVertex3f(-18.0f, 0.0f, -18.0f+i);
//            glVertex3f(18.0f, 0.0f, -18.0f+i);
//        glEnd();
//        glColor3f(1.0f, 1.0f, 1.0f);
//        glBegin(GL_LINES);
//            glVertex3f(-18.0f+i, 0.0f, -18.0f);
//            glVertex3f(-18.0f+i, 0.0f, 18.0f);
//        glEnd();
//    }

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
//    glColor3f(0.0f, 1.0f, 0.0f);
//    glBegin(GL_LINES);
//        glVertex3f(0.0f, -18.0f, 0.0f);
//        glVertex3f(0.0f, 18.0f, 0.0f);
//    glEnd();

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

void MyGLWidget::draw3D()
{
    glLineWidth(2.0f);
//    MapDataFormat* lastItem = data3D[0];
    double lotStep = 0;
    double latStep = 0;
    int lastIndex = 0;
    for (int i = 0; i < uniquePosIndex.size()-1; ++i) {
//        qDebug()<<data3D[uniquePosIndex[i]]->pos->longtitude<<data3D[uniquePosIndex[i]]->pos->latitude;
        int fileDistance = uniquePosIndex[i] - lastIndex;
        lotStep = (data3D[uniquePosIndex[i+1]]->pos->longtitude - data3D[lastIndex]->pos->longtitude) / fileDistance;
        latStep = (data3D[uniquePosIndex[i+1]]->pos->latitude - data3D[lastIndex]->pos->latitude) / fileDistance;
        int times= 0;
        for (int j = lastIndex; j <= uniquePosIndex[i]; ++j) {
            for (MetaDataFormat* point : data3D[j]->data) {
                double relativeValue = point->value/maxValue;
                if (relativeValue > 0.5)
                    glColor3d(relativeValue, relativeValue, 0.0f);
                else if (relativeValue > 0.25)
                    glColor3d(0.0, relativeValue+0.5, 0.0);
                else
                    glColor3d(0.0, relativeValue, 0.0);
                glBegin(GL_POINTS);
                    glVertex3d(data3D[lastIndex]->pos->longtitude + lotStep*times, point->height*10.0/maxHeight,
                               data3D[lastIndex]->pos->latitude + latStep*times);
                glEnd();
            }
            ++times;
        }
//        qDebug()<<lotStep<<latStep;
        lastIndex = uniquePosIndex[i];
    }
//    for(MapDataFormat* item : data3D) {
//        lotStep = (item->pos->longtitude - lastItem->pos->longtitude)/10.0;
//        latStep = (item->pos->latitude - lastItem->pos->latitude)/10.0;

//        for (int i = 0; i < 10; ++i) {
//            for (MetaDataFormat* point : lastItem->data) {
//                glColor3f(point->value/maxValue, 1.0f, 0.0f);
//                glBegin(GL_POINTS);
//                    glVertex3d(item->pos->longtitude + lotStep*i, point->value*10.0/maxValue, item->pos->latitude + latStep*i);
//                glEnd();
//            }
//        }
//        lastItem = item;
//    }

}
