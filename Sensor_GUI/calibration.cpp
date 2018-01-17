#include "calibration.h"
#include "ui_calibration.h"


MotionInst *inst=0;
using namespace std;


Calibration::Calibration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Calibration)
{
    ui->setupUi(this);
    inst=new MotionInst();
    inst->resetSensors();

    Read_Calibration_Offset();

}

void Calibration::Read_Calibration_Offset(){
    ui->Xa_Offset->setText(QString::number(inst->calibXAOffset));
    ui->Ya_Offset->setText(QString::number(inst->calibYAOffset));
    ui->Za_Offset->setText(QString::number(inst->calibZAOffset));
    ui->Xg_Offset->setText(QString::number(inst->calibXGOffset));
    ui->Yg_Offset->setText(QString::number(inst->calibYGOffset));
    ui->Zg_Offset->setText(QString::number(inst->calibZGOffset));
    ui->Temp_val->setText(QString::number(inst->temperature_read));

}



void Calibration::on_Refresh_Temp_clicked()
{
    double current_temperature;
    inst->checkTemperature(&current_temperature);
    ui->Temp_val->setText(QString::number(current_temperature));
}

Calibration::~Calibration()
{
    delete ui;
}





void Calibration::on_pushButton_3_clicked()
{
    close();
}
