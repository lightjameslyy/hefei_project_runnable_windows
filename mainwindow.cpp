#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include "QFile"
#include "QFileInfo"
#include "QScrollArea"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->dialog_site = NULL;
    this->dialog_graph = NULL;

//    QUrl url("file:///C:\\Users\\LiYazhou\\Desktop\\hefei_project-master\\mymap.html");
    QUrl url("file:///C:/workspace/qt/hefei_project_runnable_windows/mymap.html");
    ui->webView_map->setUrl(url);

    ui->listWidget->setAutoScroll(true);

    for(int i = 0; i < 6; i++)
        setDrawn(i, false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_site_clicked()
{
    if(!this->dialog_site)
    {
        dialog_site = new Dialog_Site;
    }
    //dialog_site->show();
    dialog_site->exec();
    //qDebug() << this->dialog_site->latitude;
    //qDebug()<<this->dialog_site->longitude;
    if(this->dialog_site->result() == 1)
    {
        float lat = this->dialog_site->getLatitude().toFloat();
        float lon = this->dialog_site->getLontitude().toFloat();
        this->ui->lineEdit_lon->setText(this->dialog_site->getLontitude());
        this->ui->lineEdit_lat->setText(this->dialog_site->getLatitude());
        this->setGraphPosition(lon, lat);
    }
}

void MainWindow::on_btn_graphDisplay_clicked()
{
    if(!this->dialog_graph)
        dialog_graph = new Dialog_GraphDisplay(this);

//    qDebug()<<this;
    dialog_graph->show();
}

void MainWindow::setGraphPosition(float lon, float lat)
{
    qDebug()<<lon<<lat;
    QWebFrame *frame = ui->webView_map->page()->mainFrame();
        QString method = QString("SetPosition(%1,%2)").arg(lon).arg(lat);
        frame->evaluateJavaScript(method);
}

void MainWindow::draw(QString dataDir, ColorMap::FILE_TYPE filetype)
{
    switch (filetype) {
    case ColorMap::PMPL:
//        qDebug()<<getFiletypeFromFileDir(dataDir);
        if(getFiletypeFromFileDir(dataDir) != ".pmpl")
            return;
        if(getDrawn(0))
            return;
        break;
    case ColorMap::CLH:
        if(getFiletypeFromFilePath(dataDir) != ".clh")
            return;
        if(getDrawn(1))
            return;
        break;
    case ColorMap::EXT:
        if(getFiletypeFromFileDir(dataDir) != ".ext")
            return;
        if(getDrawn(2))
            return;
        break;
    case ColorMap::LAYER:
        if(getFiletypeFromFilePath(dataDir) != ".layer")
            return;
        if(getDrawn(3))
            return;
        break;
    case ColorMap::UGM3:
        if(getFiletypeFromFileDir(dataDir) != ".ugm3")
            return;
        if(getDrawn(4))
            return;
        break;
    case ColorMap::PMPLR:
        if(getFiletypeFromFileDir(dataDir) != ".pmplr")
            return;
        if(getDrawn(5))
            return;
        break;
    default:
        break;
    }

    MapWindow *mp = new MapWindow(this);
//    qDebug()<<this;
    mp->setFiletype(filetype);
    mp->plot(dataDir, filetype);
    mp->setFixedSize(mp->width(),mp->height());

    QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
    item->setSizeHint(QSize(item->sizeHint().width(),mp->height()));
    ui->listWidget->addItem(item);

    ui->listWidget->setItemWidget(item, mp);
//    qDebug()<<mp->parent()->parent()->parent()->parent()->parent()->parent()->parent();

    switch (filetype) {
    case ColorMap::PMPL:
            setDrawn(0, true);
            break;
    case ColorMap::CLH:
        setDrawn(1, true);
            break;
    case ColorMap::EXT:
        setDrawn(2, true);
            break;
    case ColorMap::LAYER:
        setDrawn(3, true);
            break;
    case ColorMap::UGM3:
        setDrawn(4, true);
            break;
    case ColorMap::PMPLR:
        setDrawn(5, true);
            break;
    default:
        break;
    }

}

void MainWindow::setDrawn(int index, bool isDrawn)
{
    this->drawn[index] = isDrawn;
}

bool MainWindow::getDrawn(int index)
{
    return this->drawn[index];
}

QString MainWindow::getFiletypeFromFilePath(QString path)
{
    int index = path.lastIndexOf('.');
    return path.right(path.length()-index);
}

QString MainWindow::getFiletypeFromFileDir(QString dir)
{
    QDir mdir(dir);
    QFileInfoList infoList = mdir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);

    if(!infoList.size())
        return "";
    else
        return getFiletypeFromFilePath(infoList[0].absoluteFilePath());
}

void MainWindow::on_groupBox_clicked()
{

}
