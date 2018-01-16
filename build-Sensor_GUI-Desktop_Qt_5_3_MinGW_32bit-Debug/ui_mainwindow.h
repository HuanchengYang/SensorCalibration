/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGraphicsView *graphicsView;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *CaptureData_Button;
    QPushButton *Calibration_Button;
    QPushButton *Configuration_Button;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(731, 498);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        graphicsView = new QGraphicsView(centralWidget);
        graphicsView->setObjectName(QStringLiteral("graphicsView"));
        graphicsView->setGeometry(QRect(420, 20, 256, 192));
        horizontalLayoutWidget = new QWidget(centralWidget);
        horizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(0, 320, 731, 51));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        CaptureData_Button = new QPushButton(horizontalLayoutWidget);
        CaptureData_Button->setObjectName(QStringLiteral("CaptureData_Button"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(CaptureData_Button->sizePolicy().hasHeightForWidth());
        CaptureData_Button->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(CaptureData_Button);

        Calibration_Button = new QPushButton(horizontalLayoutWidget);
        Calibration_Button->setObjectName(QStringLiteral("Calibration_Button"));
        sizePolicy.setHeightForWidth(Calibration_Button->sizePolicy().hasHeightForWidth());
        Calibration_Button->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(Calibration_Button);

        Configuration_Button = new QPushButton(horizontalLayoutWidget);
        Configuration_Button->setObjectName(QStringLiteral("Configuration_Button"));
        sizePolicy.setHeightForWidth(Configuration_Button->sizePolicy().hasHeightForWidth());
        Configuration_Button->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(Configuration_Button);

        MainWindow->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        CaptureData_Button->setText(QApplication::translate("MainWindow", "Capture Data", 0));
        Calibration_Button->setText(QApplication::translate("MainWindow", "Calibration", 0));
        Configuration_Button->setText(QApplication::translate("MainWindow", "Configuration", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
