#include "dialog_graphdisplay.h"
#include "ui_dialog_graphdisplay.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

Dialog_GraphDisplay::Dialog_GraphDisplay(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_GraphDisplay)
{
    ui->setupUi(this);
}

Dialog_GraphDisplay::~Dialog_GraphDisplay()
{
    delete ui;
}

void Dialog_GraphDisplay::on_buttonBox_accepted()
{
    //mmpldata

    if (ui->radioButton->isChecked())
    {
        MainWindow *ptr = (MainWindow*)parentWidget();
        qDebug()<<"init ptr"<<ptr->getDrawn(0);
        ptr->draw("C:/workspace/qt/hefei_project-master/test_data/mmpldata/20150624_1tunnel", ColorMap::PMPL);
        qDebug()<<"after draw"<<ptr->getDrawn(0);
        ui->radioButton->setChecked(false);
    }
    //ccloudtxt
    if (ui->radioButton_2->isChecked())
    {
        MainWindow *ptr = (MainWindow*)parentWidget();
        ptr->draw("C:/workspace/qt/hefei_project-master/test_data/ccloudtxt", ColorMap::CLH);
        ui->radioButton_2->setChecked(false);
    }
    //eext
    if (ui->radioButton_3->isChecked())
    {
        MainWindow *ptr = (MainWindow*)parentWidget();
        ptr->draw("C:/workspace/qt/hefei_project-master/test_data/eext/20150929", ColorMap::EXT);
        ui->radioButton_3->setChecked(false);
    }
    //llaytxt
    if (ui->radioButton_4->isChecked())
    {
        MainWindow *ptr = (MainWindow*)parentWidget();
        ptr->draw("C:/workspace/qt/hefei_project-master/test_data/llaytxt", ColorMap::LAYER);
        ui->radioButton_4->setChecked(false);
    }
    //ppm10
    if (ui->radioButton_5->isChecked())
    {
        MainWindow *ptr = (MainWindow*)parentWidget();
        ptr->draw("C:/workspace/qt/hefei_project-master/test_data/ppm10/20150929", ColorMap::UGM3);
        ui->radioButton_5->setChecked(false);
    }
    //ppr
    if (ui->radioButton_6->isChecked())
    {
        MainWindow *ptr = (MainWindow*)parentWidget();
        ptr->draw("C:/workspace/qt/hefei_project-master/test_data/ppr/20150929", ColorMap::PMPLR);
        ui->radioButton_6->setChecked(false);
    }

}

void Dialog_GraphDisplay::on_buttonBox_rejected()
{
    this->close();
}
