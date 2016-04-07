#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QWidget>
#include <QGLWidget>
#include <QFileInfo>
#include <QVector>
#include <cmath>

struct MapPos {
    double longtitude;
    double latitude;
    MapPos(double _longtitude, double _latitude):longtitude(_longtitude),latitude(_latitude){}
    bool operator ==(MapPos* p2) {
        if (fabs(this->longtitude-p2->longtitude) <= 10e-7
                && fabs(this->latitude-p2->latitude) <= 10e-7)
            return true;
        else
            return false;
    }
};

struct MetaDataFormat {
    int height;
    double value;
    MetaDataFormat(int _height, double _value):height(_height),value(_value){}
};

struct MapDataFormat {
    MapPos* pos;
    QVector<MetaDataFormat*> data;
    MapDataFormat(MapPos* _pos, QVector<MetaDataFormat*> _data):pos(_pos),data(_data){}
};

class MyGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MyGLWidget(QWidget *parent = 0);
    MyGLWidget(QString dataDir);
    ~MyGLWidget();


    int getMaxHeight() const;
    void setMaxHeight(int value);

    double getMaxValue() const;
    void setMaxValue(double value);

    bool get3DDataParam(QFileInfoList list);

protected:
    //对3个纯虚函数的重定义
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    MapPos* getMapPosFromLine(QString s);

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

    QVector<MapDataFormat*> data3D;                  //所有数据
    QList<int> uniquePosIndex;
    int maxHeight = 0;
    double maxValue = 0;
    QString dataDir = "";
//    double minLongtitude = 180.0;
//    double minLatitude = 180.0;

    void drawAxis();
    void draw3D();
//    bool fullscreen;                                //是否全屏显示
};

#endif // MYGLWIDGET_H
