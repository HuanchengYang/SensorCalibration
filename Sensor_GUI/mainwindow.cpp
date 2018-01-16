#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_Calibration_Button_released()
{
    //Calibration new_Calibration;
    //new_Calibration.setModal(true);
   // new_Calibration.exec();
    hide();
    Calibration1= new Calibration(this);
    Calibration1->show();
}


void MainWindow::on_CaptureData_Button_released()
{
    hide();
    Datarecord1=new DataRecord(this);
    Datarecord1->show();

}
