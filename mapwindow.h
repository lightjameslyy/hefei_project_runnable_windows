#ifndef MAPWINDOW_H
#define MAPWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include "qvector.h"
#include "qcustomplot.h"
//#include "mainwindow.h"

namespace Ui {
class MapWindow;
}

namespace ColorMap {
enum FILE_TYPE{
    PMPL,
    UGM3,
    PMPLR,
    LAYER,
    CLH,
    EXT
};
}


struct DataFormat
{
    uint time;
    int height;
    QVector<int> heights;
    double value;
    DataFormat(uint _time, int _height):time(_time),height(_height){}
    DataFormat(uint _time, int _height, double _value):time(_time),height(_height),value(_value){}
    DataFormat(uint _time, QVector<int> _heights):time(_time),heights(_heights){}
};



#define TOOLTIP_SHOW_GAP    1500

class MapWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MapWindow(QWidget *parent = 0);
    ~MapWindow();
    void plot(QString dataDir, ColorMap::FILE_TYPE filetype, bool isRealtime = false, int samplingGap = 0, int tunnelCount = 1);

    //static int HISTORY_DATA = 1;
    //static int REATIME_DATA = 2;
    //根据文件名获取到采集数据的时间
    uint getTimeFromFileName(QString path);
    uint getTimeFromCLHFileAndEntry(QString fileName, QString time_hhmmss);
    uint getTimeFromLAYEREntry(QString time_yyMMddhhmmss);
    void drawSingleLine(QVector<DataFormat*> &vec);

    //绘制PMPL图像
    void drawPMPLMap(int tunnelCount = 1, bool isRealTime = false, int samplingGap = 0);
    //绘制UGM3
    void drawUGM3Map(bool isRealTime = false, int samplingGap = 0);
    //绘制PMPLR
    void drawPMPLRMap(bool isRealTime = false, int samplingGap = 0);
    //绘制LAYER
    void drawLAYERMap(bool isRealtime = false, int samplingGap = 0);
    //绘制CLH
    void drawCLHMap(bool isRealtime = false, int samplingGap = 0);
    //绘制EXT
    void drawEXTMap(bool isRealtime = false, int samplingGap = 0);

    /*
     * 解析PMPL图坐标轴参数，如果成功返回true，否则返回false。下面几个函数同理
     */
    bool PMPLAxisParam(int &xStart, int &xEnd, int &xSize, int &yStart, int &yEnd, int &ySize,QFileInfoList path, int tunnelCount = 1);
    bool UGM3AxisParam(int &xStart, int &xEnd, int &xSize, int &yStart, int &yEnd, int &ySize,QFileInfoList path);
    bool PMPLRAxisParam(int &xStart, int &xEnd, int &xSize, int &yStart, int &yEnd, int &ySize,QFileInfoList path);
    bool LAYERAxisParam(int &xStart, int &xEnd, int &xSize, int &yStart, int &yEnd, int &ySize,QString path/*QFileInfoList path*/);
    bool CLHAxisParam(int &xStart, int &xEnd, int &xSize, int &yStart, int &yEnd, int &ySize,QString path/*QFileInfoList path*/);
    bool EXTAxisParam(int &xStart, int &xEnd, int &xSize, int &yStart, int &yEnd, int &ySize,QFileInfoList path);

    void setGradientScale(QCPColorMap *colorMap);

    ColorMap::FILE_TYPE getFiletype() const;

    void setFiletype(const ColorMap::FILE_TYPE &value);

public slots:
    void updatePMPLMap();
    void updateUGM3Map();
    void updatePMPLRMap();
    void updateLAYERMap();
    void updateCLHMap();
    void updateEXTMap();
    void colorMapClicked(QCPAbstractPlottable*,QMouseEvent*);
    void customPlotClicked(QMouseEvent *event);


    //显示tooltip定时器函数
    void showToolTip();


private slots:
    void on_closeButton_clicked();

    void on_captureButton_clicked();
    
private:
    Ui::MapWindow *ui;
    QString dataDir;
    QVector<QVector<QVector<DataFormat*> > > list;  //tunnel<dir<file<line>>>
    QString lastReadFile;//上次遍历文件夹时获取到的数据
    int tunnelCount;//通道数量
    ColorMap::FILE_TYPE filetype;
};

#endif // MAPWINDOW_H
