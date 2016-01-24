#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <dialog_site.h>
#include <dialog_graphdisplay.h>
#include <QWebFrame>


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setGraphPosition(float lon, float lat);
    void draw(QString dataDir, ColorMap::FILE_TYPE filetype);
    void setDrawn(int index, bool isDrawn); //index==>[0...5]
    bool getDrawn(int index);

private slots:
    void on_btn_site_clicked();

    void on_btn_graphDisplay_clicked();

    void on_groupBox_clicked();

private:
    Ui::MainWindow *ui;
    Dialog_GraphDisplay *dialog_graph;
    Dialog_Site *dialog_site;
    bool drawn[6];      //判断6种图是否已经画出，下标从0到5依次对应PMPL,CLH,EXT,LAYER,UGM3,PMPLR

};

#endif // MAINWINDOW_H
