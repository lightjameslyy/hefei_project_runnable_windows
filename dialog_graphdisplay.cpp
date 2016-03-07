#include "dialog_graphdisplay.h"
#include "ui_dialog_graphdisplay.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>

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
    QString path;

//    qDebug()<<fileName;
    //mmpldata

    if (ui->radioButton->isChecked())
    {
        path = QFileDialog::getExistingDirectory(
                    this,
                    tr("Open Directory"),
                    "/",
                    QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks
                    );
        MainWindow *ptr = (MainWindow*)parentWidget();
        qDebug()<<"init ptr"<<ptr->getDrawn(0);
        ptr->draw(path, ColorMap::PMPL);
        qDebug()<<"after draw"<<ptr->getDrawn(0);
        ui->radioButton->setChecked(false);
    }
    //ccloudtxt
    if (ui->radioButton_2->isChecked())
    {
        path = QFileDialog::getOpenFileName(
                    this,
                    tr("Open File"),
                    "/",
                    "All Files (*.*);;PMPL File (*.pmpl);;CLH File (*.clh);;EXT File (*.ext);;LAYER File (*.layer);;PPM10 File (*.ugm3);;PPR File (*.pmplr)"
                    );
        qDebug()<<path;
        MainWindow *ptr = (MainWindow*)parentWidget();
        ptr->draw(path, ColorMap::CLH);
        ui->radioButton_2->setChecked(false);
    }
    //eext
    if (ui->radioButton_3->isChecked())
    {
        path = QFileDialog::getExistingDirectory(
                    this,
                    tr("Open Directory"),
                    "/",
                    QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks
                    );
        MainWindow *ptr = (MainWindow*)parentWidget();
        ptr->draw(path, ColorMap::EXT);
        ui->radioButton_3->setChecked(false);
    }
    //llaytxt
    if (ui->radioButton_4->isChecked())
    {
        path = QFileDialog::getOpenFileName(
                    this,
                    tr("Open File"),
                    "/",
                    "All Files (*.*);;PMPL File (*.pmpl);;CLH File (*.clh);;EXT File (*.ext);;LAYER File (*.layer);;PPM10 File (*.ugm3);;PPR File (*.pmplr)"
                    );
        MainWindow *ptr = (MainWindow*)parentWidget();
        ptr->draw(path, ColorMap::LAYER);
        ui->radioButton_4->setChecked(false);
    }
    //ppm10
    if (ui->radioButton_5->isChecked())
    {
        path = QFileDialog::getExistingDirectory(
                    this,
                    tr("Open Directory"),
                    "/",
                    QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks
                    );
        MainWindow *ptr = (MainWindow*)parentWidget();
        ptr->draw(path, ColorMap::UGM3);
        ui->radioButton_5->setChecked(false);
    }
    //ppr
    if (ui->radioButton_6->isChecked())
    {
        path = QFileDialog::getExistingDirectory(
                    this,
                    tr("Open Directory"),
                    "/",
                    QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks
                    );
        MainWindow *ptr = (MainWindow*)parentWidget();
        ptr->draw(path, ColorMap::PMPLR);
        ui->radioButton_6->setChecked(false);
    }

}

void Dialog_GraphDisplay::on_buttonBox_rejected()
{
    this->close();
}
