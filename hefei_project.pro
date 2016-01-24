#-------------------------------------------------
#
# Project created by QtCreator 2015-12-25T10:50:32
#
#-------------------------------------------------

QT       += core gui
QT       += webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = hefei_project
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dialog_graphdisplay.cpp \
    dialog_site.cpp \
    mapwindow.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    dialog_graphdisplay.h \
    dialog_site.h \
    mapwindow.h \
    qcustomplot.h

FORMS    += mainwindow.ui \
    dialog_graphdisplay.ui \
    dialog_site.ui \
    mapwindow.ui
