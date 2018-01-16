#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <calibration.h>
#include <datarecord.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_Calibration_Button_released();


    void on_CaptureData_Button_released();

private:

    Ui::MainWindow *ui;
    Calibration *Calibration1;
    DataRecord *Datarecord1;

};




#endif // MAINWINDOW_H
