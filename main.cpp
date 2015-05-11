#include "mainwindow.h"
#include <QApplication>

#ifdef Q_WS_MAC
#include "cocoainitializer.h"
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_MAC
    CocoaInitializer cocoaInitializer;
#endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
