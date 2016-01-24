#ifndef DIALOG_SITE_H
#define DIALOG_SITE_H

#include <QDialog>

namespace Ui {
class Dialog_Site;
}

class Dialog_Site : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Site(QWidget *parent = 0);
    ~Dialog_Site();
    QString getLontitude();
    QString getLatitude();


private slots:

    void on_textEdit_latitude_textChanged();

    void on_textEdit_longtitude_textChanged();

    void on_buttonBox_accepted();

private:
    Ui::Dialog_Site *ui;
    QString longitude;
    QString latitude;

};

#endif // DIALOG_SITE_H
