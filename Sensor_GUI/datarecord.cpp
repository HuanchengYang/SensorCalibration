#include "datarecord.h"
#include "ui_datarecord.h"

DataRecord::DataRecord(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataRecord)
{
    ui->setupUi(this);
}

DataRecord::~DataRecord()
{
    delete ui;
}
