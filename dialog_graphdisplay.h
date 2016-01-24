#ifndef DIALOG_GRAPHDISPLAY_H
#define DIALOG_GRAPHDISPLAY_H

#include <QDialog>
#include "mapwindow.h"

namespace Ui {
class Dialog_GraphDisplay;
}

class Dialog_GraphDisplay : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_GraphDisplay(QWidget *parent = 0);
    ~Dialog_GraphDisplay();


private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::Dialog_GraphDisplay *ui;
};

#endif // DIALOG_GRAPHDISPLAY_H
