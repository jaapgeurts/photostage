#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // TODO consider doing this
//    a.setQuitOnLastWindowClosed(false);

    // Load extra fonts
    int id = QFontDatabase::addApplicationFont(":/fonts/font-awesome.ttf");

    // Setup application details
    QCoreApplication::setOrganizationName("PhotoStage");
    QCoreApplication::setOrganizationDomain("www.github.com/photostage");
    QCoreApplication::setApplicationName("PhotoStage");

    // Load the dark style
    QFile f (":qdarkstyle/style.qss");
    if (!f.exists())
    {
        printf("Unable to set stylesheet. File qdarkstyle/style.qss not found.\n");
    }
    else
    {
        f.open(QFile::ReadOnly|QFile::Text);
        QTextStream ts(&f);
        a.setStyleSheet(ts.readAll());
    }
    MainWindow w;
    w.show();

    return a.exec();
}
