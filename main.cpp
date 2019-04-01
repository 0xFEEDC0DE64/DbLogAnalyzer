#include <QApplication>
#include <QMetaType>

#include "mainwindow.h"
#include "common.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<ScanResult>();

    MainWindow w;
    w.show();

    return a.exec();
}
