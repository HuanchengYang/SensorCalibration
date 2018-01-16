#include "mainwindow.h"
#include <QApplication>
#include <QPushButton>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QPushButton testbutton("Quit",&w);
    w.show();
    testbutton.show();

    QObject::connect(&testbutton, &QPushButton::released, &QApplication::quit);



    return a.exec();
}
