/********************************************************************************
** Form generated from reading UI file 'calibration.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALIBRATION_H
#define UI_CALIBRATION_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_Calibration
{
public:
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Calibration)
    {
        if (Calibration->objectName().isEmpty())
            Calibration->setObjectName(QStringLiteral("Calibration"));
        Calibration->resize(601, 501);
        buttonBox = new QDialogButtonBox(Calibration);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(260, 450, 301, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        retranslateUi(Calibration);
        QObject::connect(buttonBox, SIGNAL(accepted()), Calibration, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Calibration, SLOT(reject()));

        QMetaObject::connectSlotsByName(Calibration);
    } // setupUi

    void retranslateUi(QDialog *Calibration)
    {
        Calibration->setWindowTitle(QApplication::translate("Calibration", "Calibration", 0));
    } // retranslateUi

};

namespace Ui {
    class Calibration: public Ui_Calibration {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALIBRATION_H
