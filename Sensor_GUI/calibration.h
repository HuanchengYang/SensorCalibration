#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <QDialog>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <ctime>
#include <iostream>
#include <ostream>
#include <fstream>
#include "../backend/MotionInst.h"
#include <thread>
#include <chrono>
#include <stdlib.h>
#include <cstdio>
//#include "../backend/ntplib.h"
#include <sstream>




namespace Ui {
class Calibration;
}

class Calibration : public QDialog
{
    Q_OBJECT

public:
    explicit Calibration(QWidget *parent = 0);
    ~Calibration();
    void Read_Calibration_Offset();

private slots:
    void on_Refresh_Temp_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::Calibration *ui;
};

#endif // CALIBRATION_H
