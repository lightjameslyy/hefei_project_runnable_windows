#include "dialog_site.h"
#include "ui_dialog_site.h"

Dialog_Site::Dialog_Site(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_Site)
{
    ui->setupUi(this);
}

Dialog_Site::~Dialog_Site()
{
    delete ui;
}


void Dialog_Site::on_textEdit_latitude_textChanged()
{

}

void Dialog_Site::on_textEdit_longtitude_textChanged()
{
    this->longitude = this->ui->textEdit_longtitude->toPlainText();
}

QString Dialog_Site::getLatitude()
{
    return this->latitude;
}

QString Dialog_Site::getLontitude()
{
    return this->longitude;
}

void Dialog_Site::on_buttonBox_accepted()
{
    this->latitude = this->ui->textEdit_latitude->toPlainText();
    this->longitude = this->ui->textEdit_longtitude->toPlainText();
}
