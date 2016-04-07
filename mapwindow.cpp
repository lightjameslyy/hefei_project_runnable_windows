#include "mapwindow.h"
#include "ui_mapwindow.h"
#include "qtimer.h"
#include "mainwindow.h"
#include <string>
#include <algorithm>

MapWindow::MapWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MapWindow)
{
//    qDebug()<<(this->isRealTime = ((MainWindow*)this->parent())->getIsRealTime());
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint );
    this->list.push_back(QVector<QVector<DataFormat*> >());
}

MapWindow::~MapWindow()
{
    delete ui;
}

void MapWindow::plot(QString dataDir, ColorMap::FILE_TYPE filetype, bool isRealtime, int samplingGap, int tunnelCount)
{

    this->dataDir = dataDir;
    QCPColorMap *colorMap;
    switch(filetype)
    {
    case ColorMap::PMPL:
        this->drawPMPLMap(tunnelCount, isRealtime, samplingGap);
        colorMap = (QCPColorMap*)ui->customPlot->plottable(0);
        this->setGradientScale(colorMap);
        colorMap->setGradient(QCPColorGradient::gpPolar);
        break;
    case ColorMap::CLH:
        this->drawCLHMap(isRealtime, samplingGap);
        colorMap = (QCPColorMap*)ui->customPlot->plottable(0);
        this->setGradientScale(colorMap);
        colorMap->setGradient(QCPColorGradient::gpGrayscale);
        break;
    case ColorMap::EXT:
        this->drawEXTMap(isRealtime, samplingGap);
        colorMap = (QCPColorMap*)ui->customPlot->plottable(0);
        this->setGradientScale(colorMap);
        colorMap->colorScale()->setDataRange(QCPRange(0.0,3.0));
        colorMap->setGradient(QCPColorGradient::gpPolar);
        break;
    case ColorMap::LAYER:
        this->drawLAYERMap(isRealtime, samplingGap);
//        this->setGradientScale((QCPColorMap*)ui->customPlot->plottable(0));
        break;
    case ColorMap::UGM3:
        this->drawUGM3Map(isRealtime, samplingGap);
        colorMap = (QCPColorMap*)ui->customPlot->plottable(0);
        this->setGradientScale(colorMap);
        colorMap->setGradient(QCPColorGradient::gpPolar);

        break;
    case ColorMap::PMPLR:
        this->drawPMPLRMap(isRealtime, samplingGap);
        colorMap = (QCPColorMap*)ui->customPlot->plottable(0);
        this->setGradientScale(colorMap);
        colorMap->setGradient(QCPColorGradient::gpPolar);

        break;
    default:
        qDebug()<<"illegal file type!\n";
    }

    // rescale the key (x) and value (y) axes so the whole color map is visible:
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();


//    //设置tooltip 响应
//    QTimer *toolTipTimer = new QTimer(this);
//    connect(toolTipTimer, SIGNAL(timeout()),this, SLOT(showToolTip()));
//    toolTipTimer->start(1500);

}

bool MapWindow::PMPLAxisParam(int tunnelCount)
{
    QDir dir(this->dataDir);
    QFileInfoList infoList = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);
    //x坐标相关参数
    xSize = infoList.size();
    uint startTime = this->getTimeFromFileName(infoList[0].absoluteFilePath());
    uint endTime = this->getTimeFromFileName(infoList[infoList.size() - 1].absoluteFilePath());
    xStart = 0;
    xEnd = (endTime - startTime) / 3600;

    //y坐标相关参数
    QString path = infoList[0].absoluteFilePath();
    QFile file(path);
    this->getTimeFromFileName(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"文件未找到";
        return false;
    }

    QTextStream in(&file);
    in.readLine();
    in.readLine();
    in.readLine();

    int i = 0;
    yStart = 0;
    yEnd = 0;
    int height;
    double value;
    while(in.readLine() != NULL)
    {
        i++;
        in>>height>>value;
        if(i == 1)
            yStart = height;
        if(yEnd < height)
            yEnd = height;
    }
    file.close();
    ySize = i;
    return true;
}

bool MapWindow::CLHAxisParam()
{
    QFile file(this->dataDir);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"文件未找到";
        return false;
    }

    QTextStream in(&file);
//    qDebug()<<in.status();
    xStart = 0;
    xEnd = 0;
    yStart = 0;
    yEnd = 0;
    int i = 0;
    QString time_hhmmss = "";
    int numberOfClouds = 0;
    double height;
    uint startTime;
    uint endTime;
    QVector<DataFormat*> vector;
    in.readLine();
    while (in.atEnd() == false)
    {
        i++;
        DataFormat *df;
        QVector<int> heights;

        in >>time_hhmmss >>numberOfClouds;
//        qDebug()<<time_hhmmss;
        for(int j = 0; j < numberOfClouds; j++)
        {
            in >>height;
            heights.push_back((int)height);
        }
        endTime = getTimeFromCLHFileAndEntry(this->dataDir, time_hhmmss);
        if(i == 1)
            startTime = endTime;

        if(yEnd < height)
            yEnd = (int)height;

        df = new DataFormat(endTime, heights);
        vector.push_back(df);
        in.readLine();
    }
    this->list[0].push_back(vector);
    file.close();
//    qDebug()<<i;
    xSize = i;
    xEnd = (endTime - startTime) / 3600;

    ySize = yEnd / 30;  //10米 每 像素

    return true;
}

bool MapWindow::UGM3AxisParam()
{
    QDir dir(this->dataDir);
    QFileInfoList infoList = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);
    //x坐标相关参数
    xSize = infoList.size();
    uint startTime = this->getTimeFromFileName(infoList[0].absoluteFilePath());
    uint endTime = this->getTimeFromFileName(infoList[infoList.size() - 1].absoluteFilePath());
    xStart = 0;
    xEnd = (endTime - startTime) / 3600;

    //y坐标相关参数
    QString path = infoList[0].absoluteFilePath();
    QFile file(path);
    this->getTimeFromFileName(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"文件未找到";
        return false;
    }

    QTextStream in(&file);

    int i = 0;
    yStart = 0;
    yEnd = 0;
    int height;
    double value;
    while(in.atEnd() == false)
    {
        i++;
        in>>height>>value;
        if(i == 1)
            yStart = height;
        if(yEnd < height)
            yEnd = height;
        in.readLine();
    }
    file.close();
    ySize = i;
    return true;
}

bool MapWindow::PMPLRAxisParam()
{
    QDir dir(this->dataDir);
    QFileInfoList infoList = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);
    //x坐标相关参数
    xSize = infoList.size();
    uint startTime = this->getTimeFromFileName(infoList[0].absoluteFilePath());
    uint endTime = this->getTimeFromFileName(infoList[infoList.size() - 1].absoluteFilePath());
    xStart = 0;
    xEnd = (endTime - startTime) / 3600;

    //y坐标相关参数
    QString path = infoList[0].absoluteFilePath();
    QFile file(path);
    this->getTimeFromFileName(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"文件未找到";
        return false;
    }

    QTextStream in(&file);

    int i = 0;
    yStart = 0;
    yEnd = 0;
    int height;
    double value;
    while(in.atEnd() == false)
    {
        i++;
        in>>height>>value;
        if(i == 1)
            yStart = height;
        if(yEnd < height)
            yEnd = height;
        in.readLine();
    }
    file.close();
    ySize = i;
    return true;
}

bool MapWindow::LAYERAxisParam()
{
    QFile file(this->dataDir);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"文件未找到";
        return false;
    }

    QTextStream in(&file);
//    qDebug()<<in.status();

    xStart = 0;
    xEnd = 0;
    yStart = 0;
    yEnd = 0;
    int i = 0;
    uint startTime;
    uint endTime;
    QVector<DataFormat*> vector;
    in.readLine();
    while (in.atEnd() == false)
    {
        i++;
        DataFormat *df;
        QString time_yyMMddhhmmss;
        int height;
        in >>time_yyMMddhhmmss >>height;
        endTime = getTimeFromLAYEREntry(time_yyMMddhhmmss);
        if(i == 1)
            startTime = endTime;
        if(yEnd < height)
            yEnd = (int)height;
        df = new DataFormat(endTime, height);
        vector.push_back(df);
        in.readLine();
    }
    this->list[0].push_back(vector);
    file.close();
//    qDebug()<<i;
    xSize = i;
    xEnd = (endTime - startTime) / 3600;
    ySize = yEnd;  //1米 每 像素
    return true;
}

bool MapWindow::EXTAxisParam()
{
    QDir dir(this->dataDir);
    QFileInfoList infoList = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);
    //x坐标相关参数
    xSize = infoList.size();
    uint startTime = this->getTimeFromFileName(infoList[0].absoluteFilePath());
    uint endTime = this->getTimeFromFileName(infoList[infoList.size() - 1].absoluteFilePath());
    xStart = 0;
    xEnd = (endTime - startTime) / 3600;

    //y坐标相关参数
    QString path = infoList[0].absoluteFilePath();
    QFile file(path);
    this->getTimeFromFileName(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"文件未找到";
        return false;
    }

    QTextStream in(&file);

    int i = 0;
    yStart = 0;
    yEnd = 0;
    int height;
    double value;
    while(in.atEnd() == false)
    {
        i++;
        in>>height>>value;
        if(i == 1)
            yStart = height;
        if(yEnd < height)
            yEnd = height;
        in.readLine();
    }
    file.close();
    ySize = i;
    return true;
}

uint MapWindow::getTimeFromFileName(QString path)
{
    int formatIndex = path.lastIndexOf('.');
    //补齐年份的前两位
    QString str = "20" + path.mid(formatIndex - 12, 12);
    QDateTime time = QDateTime::fromString(str, "yyyyMMddhhmmss");
    return time.toTime_t();

    /*e.g.:
     *"..../AG-MPL_150116000018.pmpl"-->20150116000018
     */

}

uint MapWindow::getTimeFromCLHFileAndEntry(QString fileName, QString time_hhmmss)
{

    int index = fileName.lastIndexOf('.');
    QString str = fileName.mid(index - 8, 8);   //20150929

    QString str1 = time_hhmmss.remove(':');     //000017

    QDateTime time = QDateTime::fromString(str+str1, "yyyyMMddhhmmss");
    return time.toTime_t();

    /*
     * e.g.:
     * fileName:20150929.clh
     * time_hhmmss:00:00:17
     */
}

uint MapWindow::getTimeFromLAYEREntry(QString time_yyMMddhhmmss)
{
    QString str = "20" + time_yyMMddhhmmss;

    QDateTime time = QDateTime::fromString(str, "yyyyMMddhhmmss");
    return time.toTime_t();

    /*
     * e.g.:
     * time_yyMMddhhmmss:150929000017
     */
}

void MapWindow::colorMapClicked(QCPAbstractPlottable *plottable, QMouseEvent *event)
{
    //qDebug()<<event->globalX()<<event->globalY();
    // qDebug()<<event->x()<<event->y();
    QCPColorMap *colorMap = (QCPColorMap*)plottable;


}

void MapWindow::customPlotClicked(QMouseEvent *event)
{
    int lowerBound, upperBound, xPos;
    lowerBound = ui->customPlot->axisRect(0)->left();
    upperBound = ui->customPlot->axisRect(0)->right();
    xPos = event->x();


    if(xPos < lowerBound || xPos > upperBound)
    {
        return;
    }
    else
    {
        double rate = (double)(xPos - lowerBound) / (double)(upperBound - lowerBound);
        int index = this->list[0].size() * rate;
        this->drawSingleLine(this->list[0][index]);


    }
}

void MapWindow::drawSingleLine(QVector<DataFormat *> &vec)
{
    if(!ui->singleLinePlot->isVisible())
        ui->singleLinePlot->setVisible(true);
    else
        ui->singleLinePlot->graph(0)->clearData();

    ui->singleLinePlot->addGraph();
    ui->singleLinePlot->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
    ui->singleLinePlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue
   // ui->singleLinePlot->graph(0)->data()->set
    ui->singleLinePlot->addGraph();

    QVector<double> x(vec.size()), y(vec.size());
    for(int i = 0; i < vec.size() ; i++)
    {
        x[i] = (double)vec[i]->height / 1000;
        y[i] = (double)vec[i]->value;
    }
    ui->singleLinePlot->xAxis->setLabel("单位：千米");
    ui->singleLinePlot->graph(0)->setData(x, y);
    ui->singleLinePlot->graph(0)->rescaleAxes(true);
    ui->singleLinePlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->singleLinePlot->replot();

}

void MapWindow::showToolTip()
{
//    qDebug()<<QCursor::pos().x()<<QCursor::pos().y();
//    QToolTip::showText(QCursor::pos(), "hahaha");
}

void MapWindow::drawPMPLMap(int tunnelCount , bool isRealTime , int samplingGap)
{
    ui->label->setText("pmpl");

    //根据dir得到.pmpl文件列表
    QDir dir(this->dataDir);
    QFileInfoList infoList = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);

    ui->singleLinePlot->setVisible(false);

    // configure axis rect:
    ui->customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
    ui->customPlot->axisRect()->setupFullAxesBox(true);
    ui->customPlot->xAxis->setLabel("时间");
    ui->customPlot->yAxis->setLabel("高度（m）");

    //获取横纵坐标相关参数
//    int xStart, xEnd, xSize, ySize, yStart, yEnd;
    this->PMPLAxisParam();

    // set up the QCPColorMap:
    QCPColorMap *colorMap = new QCPColorMap(ui->customPlot->xAxis, ui->customPlot->yAxis);
    ui->customPlot->addPlottable(colorMap);

    //注册该显示图的点击处理函数
    connect(ui->customPlot,    SIGNAL(plottableClick(QCPAbstractPlottable*,QMouseEvent*)),
       this, SLOT(colorMapClicked(QCPAbstractPlottable*,QMouseEvent*)));
    connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(customPlotClicked(QMouseEvent*)));

    colorMap->data()->setSize(xSize, ySize); // we want the color map to have nx * ny data points
    colorMap->data()->setRange(QCPRange(xStart, xEnd), QCPRange(yStart, yEnd)); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions

    // now we assign some data, by accessing the QCPColorMapData instance of the color map:
    for (int xIndex=0; xIndex<xSize; ++xIndex)
    {
        QFile file(infoList[xIndex].absoluteFilePath());
        uint time = this->getTimeFromFileName(infoList[xIndex].absoluteFilePath());
        DataFormat *df;
        QVector<DataFormat*> vector;
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
             return;
        QTextStream in(&file);
        in.readLine();
        in.readLine();
        in.readLine();
        int yIndex = 0;
        while(in.atEnd() == false)
        {
            int i;
            double d;
            in>>i>>d;
//            qDebug()<<d;
            df = new DataFormat(time, i, d);
            vector.push_back(df);
            colorMap->data()->setCell(xIndex, yIndex, d);
            yIndex++;
            in.readLine();
        }
        file.close();
        this->list[0].push_back(vector);
    }

    colorMap->rescaleDataRange();
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();

    this->lastFilePos = xSize;
//    this->lastReadFile = infoList[xSize - 1].absoluteFilePath();

    if(isRealTime)
    {
        QTimer *dataTimer = new QTimer(this);
        connect(dataTimer, SIGNAL(timeout()), this, SLOT(updatePMPLMap()));
        dataTimer->start(samplingGap);
    }
}

void MapWindow::drawCLHMap(bool isRealTime, int samplingGap)
{
    ui->label->setText("clh");
    //得到.clh文件路径
    QString path = this->dataDir;

    ui->singleLinePlot->setVisible(false);

    // configure axis rect:
    ui->customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
    ui->customPlot->axisRect()->setupFullAxesBox(true);
    ui->customPlot->xAxis->setLabel("时间");
    ui->customPlot->yAxis->setLabel("高度（m）");
//    ui->customPlot->setBackground(QBrush(Qt::white));

    //获取横纵坐标相关参数
//    int xStart, xEnd, xSize, ySize, yStart, yEnd;
    this->CLHAxisParam();
    lastFilePos = xSize;

    // set up the QCPColorMap:
    QCPColorMap *colorMap = new QCPColorMap(ui->customPlot->xAxis, ui->customPlot->yAxis);
    ui->customPlot->addPlottable(colorMap);

//    colorMap->setPen(QPen(Qt::white));
//    colorMap->setBrush(Qt::white);

    colorMap->data()->setSize(xSize, ySize); // we want the color map to have nx * ny data points
    colorMap->data()->setRange(QCPRange(xStart, xEnd), QCPRange(yStart, yEnd)); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions

    for(int xIndex = 0; xIndex < xSize; xIndex++)
    {
        DataFormat *df = this->list[0][0][xIndex];
        QVector<int> heights = df->heights;
        for(int i = 0; i < heights.length(); i++)
            colorMap->data()->setCell(xIndex, heights[i] / 30, -1);
    }
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();

    if(isRealTime)
    {
        QTimer *dataTimer = new QTimer(this);
        connect(dataTimer, SIGNAL(timeout()), this, SLOT(updateCLHMap()));
        dataTimer->start(samplingGap);
    }
}

void MapWindow::drawUGM3Map(bool isRealTime, int samplingGap)
{
    ui->label->setText("ugm3");

    //根据dir得到.ugm3文件列表;
    QDir dir(this->dataDir);
    QFileInfoList infoList = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);

    ui->singleLinePlot->setVisible(false);

    // configure axis rect:
    ui->customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
    ui->customPlot->axisRect()->setupFullAxesBox(true);
    ui->customPlot->xAxis->setLabel("时间");
    ui->customPlot->yAxis->setLabel("高度（m）");
    //ui->customPlot->setBackground(QBrush(QColor(0, 255, 0, 100)));

    //获取横纵坐标相关参数
//    int xStart, xEnd, xSize, ySize, yStart, yEnd;
    this->UGM3AxisParam();

    // set up the QCPColorMap:
    QCPColorMap *colorMap = new QCPColorMap(ui->customPlot->xAxis, ui->customPlot->yAxis);
    ui->customPlot->addPlottable(colorMap);

    //注册该显示图的点击处理函数
    connect(ui->customPlot,    SIGNAL(plottableClick(QCPAbstractPlottable*,QMouseEvent*)),
       this, SLOT(colorMapClicked(QCPAbstractPlottable*,QMouseEvent*)));
    connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(customPlotClicked(QMouseEvent*)));

    colorMap->data()->setSize(xSize, ySize); // we want the color map to have nx * ny data points
    colorMap->data()->setRange(QCPRange(xStart, xEnd), QCPRange(yStart, yEnd)); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions

    // now we assign some data, by accessing the QCPColorMapData instance of the color map:
    for (int xIndex=0; xIndex<xSize; ++xIndex)
    {
        QFile file(infoList[xIndex].absoluteFilePath());
        uint time = this->getTimeFromFileName(infoList[xIndex].absoluteFilePath());
        DataFormat *df;
        QVector<DataFormat*> vector;
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
             return;
        QTextStream in(&file);
        int yIndex = 1;
        while(in.atEnd() == false)
        {
            int i;
            double d;
            in>>i>>d;
            df = new DataFormat(time, i, d);
            vector.push_back(df);
            colorMap->data()->setCell(xIndex, yIndex, d);
            yIndex++;
            in.readLine();
        }
        file.close();
        this->list[0].push_back(vector);
    }
    this->lastFilePos = xSize;
//    this->lastReadFile = infoList[xSize - 1].absoluteFilePath();

    colorMap->rescaleDataRange();
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();

    if(isRealTime)
    {
        QTimer *dataTimer = new QTimer(this);
        connect(dataTimer, SIGNAL(timeout()), this, SLOT(updateUGM3Map()));
        dataTimer->start(samplingGap);
    }
}

void MapWindow::drawPMPLRMap(bool isRealTime, int samplingGap)
{
    ui->label->setText("pmplr");

    //根据dir得到.pmplr文件列表
    QDir dir(this->dataDir);
    QFileInfoList infoList = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);

    ui->singleLinePlot->setVisible(false);

    // configure axis rect:
    ui->customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
    ui->customPlot->axisRect()->setupFullAxesBox(true);
    ui->customPlot->xAxis->setLabel("时间");
    ui->customPlot->yAxis->setLabel("高度（m）");
    //ui->customPlot->setBackground(QBrush(QColor(0, 255, 0, 100)));

    //获取横纵坐标相关参数
//    int xStart, xEnd, xSize, ySize, yStart, yEnd;
    this->PMPLRAxisParam();

    // set up the QCPColorMap:
    QCPColorMap *colorMap = new QCPColorMap(ui->customPlot->xAxis, ui->customPlot->yAxis);
    ui->customPlot->addPlottable(colorMap);

    //注册该显示图的点击处理函数
    connect(ui->customPlot,    SIGNAL(plottableClick(QCPAbstractPlottable*,QMouseEvent*)),
       this, SLOT(colorMapClicked(QCPAbstractPlottable*,QMouseEvent*)));
    connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(customPlotClicked(QMouseEvent*)));

    colorMap->data()->setSize(xSize, ySize); // we want the color map to have nx * ny data points
    colorMap->data()->setRange(QCPRange(xStart, xEnd), QCPRange(yStart, yEnd)); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions

    // now we assign some data, by accessing the QCPColorMapData instance of the color map:
    for (int xIndex=0; xIndex<xSize; ++xIndex)
    {
        QFile file(infoList[xIndex].absoluteFilePath());
        uint time = this->getTimeFromFileName(infoList[xIndex].absoluteFilePath());
        DataFormat *df;
        QVector<DataFormat*> vector;
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
             return;
        QTextStream in(&file);
        int yIndex = 0;
        while(in.atEnd() == false)
        {
            int i;
            double d;
            in>>i>>d;
            df = new DataFormat(time, i, d);
            vector.push_back(df);
            colorMap->data()->setCell(xIndex, yIndex, d);
            yIndex++;
            in.readLine();
        }
        file.close();
        this->list[0].push_back(vector);
    }
    colorMap->rescaleDataRange();
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();

    this->lastFilePos = xSize;
//    this->lastReadFile = infoList[xSize - 1].absoluteFilePath();

    if(isRealTime)
    {
        QTimer *dataTimer = new QTimer(this);
        connect(dataTimer, SIGNAL(timeout()), this, SLOT(updatePMPLRMap()));
        dataTimer->start(samplingGap);
    }
}

void MapWindow::drawLAYERMap(bool isRealTime, int samplingGap)
{
    ui->label->setText("layer");
    ui->singleLinePlot->setVisible(false);

    // configure axis rect:
    ui->customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
    ui->customPlot->axisRect()->setupFullAxesBox(true);
    ui->customPlot->xAxis->setLabel("时间");
    ui->customPlot->yAxis->setLabel("高度（m）");
//    ui->customPlot->setBackground(QBrush(Qt::white));

    //获取横纵坐标相关参数
//    int xStart, xEnd, xSize, ySize, yStart, yEnd;
    this->LAYERAxisParam();
    this->lastFilePos = xSize;

    QVector<double> x(xSize),y(xSize);
    for(int i = 0; i < xSize; i++)
    {
        x[i] = (double)i;
        y[i] = (double)this->list[0][0][i]->height;
    }

    ui->customPlot->addGraph();
//    ui->customPlot->graph(0)->setData();
    ui->customPlot->graph(0)->setData(x,y);
    ui->customPlot->graph(0)->setPen(QPen(Qt::red));
    ui->customPlot->xAxis->setRange(xStart, xEnd);
    ui->customPlot->yAxis->setRange(yStart, yEnd);
    ui->customPlot->graph(0)->rescaleAxes(true);
    ui->customPlot->replot();

    if(isRealTime)
    {
        QTimer *dataTimer = new QTimer(this);
        connect(dataTimer, SIGNAL(timeout()), this, SLOT(updateLAYERMap()));
        dataTimer->start(samplingGap);
    }
}

void MapWindow::drawEXTMap(bool isRealTime, int samplingGap)
{
    ui->label->setText("ext");

//    qDebug()<<isRealtime;
    //根据dir得到.ext文件列表
    QDir dir(this->dataDir);
    QFileInfoList infoList = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);

    ui->singleLinePlot->setVisible(false);

    // configure axis rect:
    ui->customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
    ui->customPlot->axisRect()->setupFullAxesBox(true);
    ui->customPlot->xAxis->setLabel("时间");
    ui->customPlot->yAxis->setLabel("高度（m）");

    //获取横纵坐标相关参数
//    int xStart, xEnd, xSize, ySize, yStart, yEnd;
    this->EXTAxisParam();

    // set up the QCPColorMap:
    QCPColorMap *colorMap = new QCPColorMap(ui->customPlot->xAxis, ui->customPlot->yAxis);
    ui->customPlot->addPlottable(colorMap);

    //注册该显示图的点击处理函数
    connect(ui->customPlot,    SIGNAL(plottableClick(QCPAbstractPlottable*,QMouseEvent*)),
       this, SLOT(colorMapClicked(QCPAbstractPlottable*,QMouseEvent*)));
    connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(customPlotClicked(QMouseEvent*)));

    colorMap->data()->setSize(xSize, ySize); // we want the color map to have nx * ny data points
    colorMap->data()->setRange(QCPRange(xStart, xEnd), QCPRange(yStart, yEnd)); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions

    // now we assign some data, by accessing the QCPColorMapData instance of the color map:
    for (int xIndex=0; xIndex<xSize; ++xIndex)
    {
        QFile file(infoList[xIndex].absoluteFilePath());
        uint time = this->getTimeFromFileName(infoList[xIndex].absoluteFilePath());
        DataFormat *df;
        QVector<DataFormat*> vector;
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
             return;
        QTextStream in(&file);
        int yIndex = 0;
        while(in.atEnd() == false)
        {
            int i;
            double d;
            in>>i>>d;
//            qDebug()<<d;
            df = new DataFormat(time, i, d);
            vector.push_back(df);
            colorMap->data()->setCell(xIndex, yIndex, d);
            yIndex++;
            in.readLine();
        }
        file.close();
        this->list[0].push_back(vector);
    }

    this->lastFilePos = xSize;
    colorMap->rescaleDataRange();
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();

//    this->lastReadFile = infoList[xSize - 1].absoluteFilePath();

    if(isRealTime)
    {
        QTimer *dataTimer = new QTimer(this);
        connect(dataTimer, SIGNAL(timeout()), this, SLOT(updateEXTMap()));
        dataTimer->start(samplingGap);
    }
}

void MapWindow::setGradientScale(QCPColorMap *colorMap )
{
    // add a color scale:
    QCPColorScale *colorScale = new QCPColorScale(ui->customPlot);
    ui->customPlot->plotLayout()->addElement(0, 1, colorScale); // add it to the right of the main axis rect
    colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
    colorMap->setColorScale(colorScale); // associate the color map with the color scale
//    colorScale->axis()->setLabel("Magnetic Field Strength");

    // set the color gradient of the color map to one of the presets:
//    colorMap->setGradient(QCPColorGradient::gpGrayscale);

    // we could have also created a QCPColorGradient instance and added own colors to
    // the gradient, see the documentation of QCPColorGradient for what's possible.

    // rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient:
    colorMap->rescaleDataRange();

    // make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
    QCPMarginGroup *marginGroup = new QCPMarginGroup(ui->customPlot);
    ui->customPlot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
}

void MapWindow::updatePMPLMap()
{
    QDir dir(this->dataDir);
    QFileInfoList infoList = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);
    if (infoList.size() <= xSize) return;
    qDebug()<<"update PMPL";
//    if(this->lastReadFile.compare(infoList[infoList.size() - 1].absoluteFilePath()) >= 0)
//        return;

    QCPColorMap *colorMap = (QCPColorMap*)ui->customPlot->plottable(0);

    //获取横纵坐标相关参数
//    int newxStart, newxEnd, newxSize, newySize, newyStart, newyEnd;
    this->PMPLAxisParam();
    infoList = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);
    // set up the QCPColorMap:
    colorMap->data()->setSize(xSize, ySize); // we want the color map to have nx * ny data points
    colorMap->data()->setRange(QCPRange(xStart, xEnd), QCPRange(yStart, yEnd)); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions
    for (int xIndex=0; xIndex<xSize; ++xIndex)
    {
        if (xIndex < lastFilePos) {
            QVector<DataFormat*> vector = this->list[0][xIndex];
            for (int i = 0; i < vector.size(); ++i) {
                colorMap->data()->setCell(xIndex, i, vector[i]->value);
            }
        }
        else {
            QString filePath = infoList[xIndex].absoluteFilePath();
            QFile file(filePath);
            QVector<DataFormat*> vector;
            DataFormat* df;
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                 return;
            QTextStream in(&file);
            in.readLine();
            in.readLine();
            in.readLine();
            int yIndex = 0;
            while(in.atEnd() == false)
            {
                int i;
                double d;
                in>>i>>d;
                colorMap->data()->setCell(xIndex, yIndex, d);
                df = new DataFormat(this->getTimeFromFileName(filePath), i, d);
                vector.push_back(df);
                yIndex++;
                in.readLine();
            }
            this->list[0].push_back(vector);
            file.close();
        }
    }
    this->lastFilePos = xSize;
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();
    qDebug() <<"PMPL updated";
}

void MapWindow::updateCLHMap()
{
    this->list[0].clear();
    CLHAxisParam();
    if (xSize <= lastFilePos) return;
    qDebug() << "update CLH";
    QCPColorMap *colorMap = (QCPColorMap*)ui->customPlot->plottable(0);
    colorMap->data()->setSize(xSize, ySize); // we want the color map to have nx * ny data points
    colorMap->data()->setRange(QCPRange(xStart, xEnd), QCPRange(yStart, yEnd)); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions

    for(int xIndex = 0; xIndex < xSize; xIndex++)
    {
        DataFormat *df = this->list[0][0][xIndex];
        QVector<int> heights = df->heights;
        for(int i = 0; i < heights.length(); i++)
            colorMap->data()->setCell(xIndex, heights[i] / 30, -1);
    }
    this->lastFilePos = xSize;
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();
    qDebug() <<"CLH updated";
}

void MapWindow::updateEXTMap()
{
    QDir dir(this->dataDir);
    QFileInfoList infoList = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);
    if (infoList.size() <= xSize) return;
    qDebug()<<"update EXT";

    QCPColorMap *colorMap = (QCPColorMap*)ui->customPlot->plottable(0);

    //获取横纵坐标相关参数
//    int newxStart, newxEnd, newxSize, newySize, newyStart, newyEnd;
    this->EXTAxisParam();
    infoList = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);
    // set up the QCPColorMap:
    colorMap->data()->setSize(xSize, ySize); // we want the color map to have nx * ny data points
    colorMap->data()->setRange(QCPRange(xStart, xEnd), QCPRange(yStart, yEnd)); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions
    for (int xIndex=0; xIndex<xSize; ++xIndex)
    {
        if (xIndex < lastFilePos) {
            QVector<DataFormat*> vector = this->list[0][xIndex];
            for (int i = 0; i < vector.size(); ++i) {
                colorMap->data()->setCell(xIndex, i, vector[i]->value);
            }
        }
        else {
            QString filePath = infoList[xIndex].absoluteFilePath();
            QFile file(filePath);
            QVector<DataFormat*> vector;
            DataFormat* df;
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                 return;
            QTextStream in(&file);
            int yIndex = 0;
            while(in.atEnd() == false)
            {
                int i;
                double d;
                in>>i>>d;
                colorMap->data()->setCell(xIndex, yIndex, d);
                df = new DataFormat(this->getTimeFromFileName(filePath), i, d);
                vector.push_back(df);
                yIndex++;
                in.readLine();
            }
            this->list[0].push_back(vector);
            file.close();
        }
    }
    this->lastFilePos = xSize;
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();
    qDebug() <<"EXT updated";
}

void MapWindow::updateLAYERMap()
{
    qDebug() << "enter updateLAYERMap";
    this->list[0].clear();
    LAYERAxisParam();
    qDebug() << xSize << lastFilePos;
    if (xSize <= lastFilePos) return;
    lastFilePos = xSize;
    qDebug() << "update LAYER";
    QVector<double> x(xSize),y(xSize);
    for(int i = 0; i < xSize; i++)
    {
        x[i] = (double)i;
        y[i] = (double)this->list[0][0][i]->height;
    }

    ui->customPlot->graph(0)->data()->clear();
    ui->customPlot->graph(0)->setData(x,y);
    ui->customPlot->graph(0)->setPen(QPen(Qt::red));
    ui->customPlot->xAxis->setRange(xStart, xEnd);
    ui->customPlot->yAxis->setRange(yStart, yEnd);
    ui->customPlot->graph(0)->rescaleAxes(true);
    ui->customPlot->replot();
    qDebug() <<"LAYER updated";
}

void MapWindow::updatePMPLRMap()
{
    QDir dir(this->dataDir);
    QFileInfoList infoList = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);
    if (infoList.size() <= xSize) return;
    qDebug()<<"update PMPLR";
//    qDebug() << this->dataDir;
    QCPColorMap *colorMap = (QCPColorMap*)ui->customPlot->plottable(0);

    //获取横纵坐标相关参数
//    int newxStart, newxEnd, newxSize, newySize, newyStart, newyEnd;
    this->PMPLRAxisParam();
    infoList = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);
    // set up the QCPColorMap:
    colorMap->data()->setSize(xSize, ySize); // we want the color map to have nx * ny data points
    colorMap->data()->setRange(QCPRange(xStart, xEnd), QCPRange(yStart, yEnd)); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions
    for (int xIndex=0; xIndex<xSize; ++xIndex)
    {
        if (xIndex < lastFilePos) {
            QVector<DataFormat*> vector = this->list[0][xIndex];
            for (int i = 0; i < vector.size(); ++i) {
                colorMap->data()->setCell(xIndex, i, vector[i]->value);
            }
        }
        else {
            QString filePath = infoList[xIndex].absoluteFilePath();
            QFile file(filePath);
            QVector<DataFormat*> vector;
            DataFormat* df;
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                 return;
            QTextStream in(&file);
            int yIndex = 0;
            while(in.atEnd() == false)
            {
                int i;
                double d;
                in>>i>>d;
                colorMap->data()->setCell(xIndex, yIndex, d);
                df = new DataFormat(this->getTimeFromFileName(filePath), i, d);
                vector.push_back(df);
                yIndex++;
                in.readLine();
            }
            this->list[0].push_back(vector);
            file.close();
        }
    }
    this->lastFilePos = xSize;
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();
    qDebug() <<"PMPLR updated";
}

void MapWindow::updateUGM3Map()
{
    QDir dir(this->dataDir);
    QFileInfoList infoList = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);
    if (infoList.size() <= xSize) return;
    qDebug()<<"update UGM3";
    QCPColorMap *colorMap = (QCPColorMap*)ui->customPlot->plottable(0);

    //获取横纵坐标相关参数
//    int newxStart, newxEnd, newxSize, newySize, newyStart, newyEnd;
    this->PMPLRAxisParam();
    infoList = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);
    // set up the QCPColorMap:
    colorMap->data()->setSize(xSize, ySize); // we want the color map to have nx * ny data points
    colorMap->data()->setRange(QCPRange(xStart, xEnd), QCPRange(yStart, yEnd)); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions
    for (int xIndex=0; xIndex<xSize; ++xIndex)
    {
        if (xIndex < lastFilePos) {
            QVector<DataFormat*> vector = this->list[0][xIndex];
            for (int i = 0; i < vector.size(); ++i) {
                colorMap->data()->setCell(xIndex, i, vector[i]->value);
            }
        }
        else {
            QString filePath = infoList[xIndex].absoluteFilePath();
            QFile file(filePath);
            QVector<DataFormat*> vector;
            DataFormat* df;
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                 return;
            QTextStream in(&file);
            int yIndex = 0;
            while(in.atEnd() == false)
            {
                int i;
                double d;
                in>>i>>d;
                colorMap->data()->setCell(xIndex, yIndex, d);
                df = new DataFormat(this->getTimeFromFileName(filePath), i, d);
                vector.push_back(df);
                yIndex++;
                in.readLine();
            }
            this->list[0].push_back(vector);
            file.close();
        }
    }
    this->lastFilePos = xSize;
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();
    qDebug() <<"UGM3 updated";
}

void MapWindow::on_closeButton_clicked()
{
//    qDebug()<<this->parent()->parent();
    MainWindow *mw = (MainWindow*)this->parent()->parent()->parent()->parent()->parent()->parent()->parent();
    switch (this->filetype) {
    case ColorMap::PMPL:
        mw->setDrawn(0, false);
        break;
    case ColorMap::CLH:
        mw->setDrawn(1, false);
        break;
    case ColorMap::EXT:
        mw->setDrawn(2, false);
        break;
    case ColorMap::LAYER:
        mw->setDrawn(3, false);
        break;
    case ColorMap::UGM3:
        mw->setDrawn(4, false);
        break;
    case ColorMap::PMPLR:
        mw->setDrawn(5, false);
        break;
    default:
        break;
    }
    qDebug()<<"close"<<mw->getDrawn(0);

    QListWidget *qlw = (QListWidget*)this->parent()->parent();
    int row = 0;
    while(QListWidgetItem *item = qlw->item(row))
    {
        if(qlw->itemWidget(item) == this)
            qlw->takeItem(row);
        row++;
    }
}

void MapWindow::setFiletype(const ColorMap::FILE_TYPE &value)
{
    filetype = value;
}

ColorMap::FILE_TYPE MapWindow::getFiletype() const
{
    return filetype;
}

void MapWindow::on_captureButton_clicked()
{
    int fileNameLength = dataDir.size() - dataDir.lastIndexOf('/')-1;
    QString defaultFileName = dataDir.right(fileNameLength);
    qDebug()<<defaultFileName;
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               "/"+defaultFileName+".png",
                               tr("Images (*.png)"));
    if (!fileName.isNull()) {
        qDebug()<<fileName;
        QPixmap capture = this->grab(this->rect());
        capture.save(fileName, "png");
    }

}
