/********************************************************************************
** Form generated from reading UI file 'datarecord.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DATARECORD_H
#define UI_DATARECORD_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_DataRecord
{
public:
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DataRecord)
    {
        if (DataRecord->objectName().isEmpty())
            DataRecord->setObjectName(QStringLiteral("DataRecord"));
        DataRecord->resize(320, 240);
        buttonBox = new QDialogButtonBox(DataRecord);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(10, 200, 301, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        retranslateUi(DataRecord);
        QObject::connect(buttonBox, SIGNAL(accepted()), DataRecord, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DataRecord, SLOT(reject()));

        QMetaObject::connectSlotsByName(DataRecord);
    } // setupUi

    void retranslateUi(QDialog *DataRecord)
    {
        DataRecord->setWindowTitle(QApplication::translate("DataRecord", "Dialog", 0));
    } // retranslateUi

};

namespace Ui {
    class DataRecord: public Ui_DataRecord {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DATARECORD_H
