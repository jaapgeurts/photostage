#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

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
