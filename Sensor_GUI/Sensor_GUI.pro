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
    datarecord.cpp \
    ../backend/I2Cdev.cpp \
    ../backend/MPU6050.cpp \
    ../backend/INIReader.cpp \
    ../backend/MotionInst.cpp \
    ../backend/ini.c

HEADERS  += mainwindow.h \
    calibration.h \
    datarecord.h \
    ../backend/helper_3dmath.h \
    ../backend/I2Cdev.h \
    ../backend/ini.h \
    ../backend/INIReader.h \
    ../backend/MPU6050.h \
    ../backend/MPU6050_6Axis_MotionApps20.h \
    ../backend/ntplib.h \
    ../backend/MotionInst.h
    ../backend/MotionInst.h \


FORMS    += mainwindow.ui \
    calibration.ui \
    datarecord.ui

DISTFILES +=

QMAKE_CXXFLAGS += -std=c++11
