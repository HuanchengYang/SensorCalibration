#ifndef DATARECORD_H
#define DATARECORD_H

#include <QDialog>

namespace Ui {
class DataRecord;
}

class DataRecord : public QDialog
{
    Q_OBJECT

public:
    explicit DataRecord(QWidget *parent = 0);
    ~DataRecord();

private:
    Ui::DataRecord *ui;
};

#endif // DATARECORD_H
