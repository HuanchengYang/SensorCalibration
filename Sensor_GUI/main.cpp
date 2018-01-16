#include "mainwindow.h"
#include <QApplication>
#include <QPushButton>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QPushButton button("Hello World!",&w);
    w.show();


    button.show();

    return a.exec();
}
