#-------------------------------------------------
#
# Project created by QtCreator 2018-01-15T15:15:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Sensor_GUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    calibration.cpp \
    datarecord.cpp

HEADERS  += mainwindow.h \
    calibration.h \
    datarecord.h \
    MotionInst.h

FORMS    += mainwindow.ui \
    calibration.ui \
    datarecord.ui

DISTFILES +=
