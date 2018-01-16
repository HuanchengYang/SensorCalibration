#include "calibration.h"
#include "ui_calibration.h"
//#include "Motionlnst.h"

Calibration::Calibration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Calibration)
{
    ui->setupUi(this);
}

Calibration::~Calibration()
{
    delete ui;
}
