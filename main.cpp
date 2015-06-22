#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>
#include <QDebug>

#include <QMetaType>

#include <lcms2.h>

void lcms2ErrorFunc(cmsContext ContextID, cmsUInt32Number ErrorCode,
    const char* Text)
{
    qDebug() << "Little CMS error(" << ErrorCode << ")" << Text;
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    // TODO consider doing this
    //    a.setQuitOnLastWindowClosed(false);

    // Load extra fonts
    /*int id =*/ //QFontDatabase::addApplicationFont(":/fonts/font-awesome.ttf");
    int         id = QFontDatabase::addApplicationFont(
        ":/fonts/foundation-icons-general.ttf");
    QStringList l = QFontDatabase::applicationFontFamilies(id);

    qDebug() << "Loaded fonts:" << l;

    // Setup application details
    QCoreApplication::setOrganizationName("PhotoStage");
    QCoreApplication::setOrganizationDomain("www.github.com/photostage");
    QCoreApplication::setApplicationName("PhotoStage");

    // Load the dark style
#define STYLE_SHEET 1
#ifdef STYLE_SHEET
    QFile f (":qdarkstyle/style.qss");

    if (!f.exists())
    {
        printf(
            "Unable to set stylesheet. File qdarkstyle/style.qss not found.\n");
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        a.setStyleSheet(ts.readAll());
    }
#else
    QPalette darkPalette = QApplication::palette();
    darkPalette.setColor(QPalette::Normal, QPalette::Window, QColor(Qt::black));
    darkPalette.setColor(QPalette::Normal, QPalette::Base, QColor(Qt::black));
    darkPalette.setColor(QPalette::Normal, QPalette::WindowText,
        QColor(Qt::lightGray));
    darkPalette.setColor(QPalette::Normal, QPalette::Text,
        QColor(Qt::lightGray));
    darkPalette.setColor(QPalette::Normal, QPalette::Button, QColor(Qt::black));
    darkPalette.setColor(QPalette::Normal, QPalette::ButtonText,
        QColor(Qt::lightGray));
    darkPalette.setColor(QPalette::Normal, QPalette::Light, QColor(
            Qt::lightGray).lighter(180));
    darkPalette.setColor(QPalette::Normal, QPalette::Midlight,
        QColor(Qt::lightGray).lighter(150));
    darkPalette.setColor(QPalette::Normal, QPalette::Dark, QColor(
            Qt::darkGray).lighter(150));
    darkPalette.setColor(QPalette::Normal, QPalette::Mid, QColor(
            Qt::darkGray).lighter(120));
    darkPalette.setColor(QPalette::Normal, QPalette::Shadow, QColor(
            Qt::darkGray));

    QApplication::setPalette(darkPalette);
#endif

    // Register the Image type so that we can use it in signals and slots
    qRegisterMetaType<PhotoStage::Image>("Image");

    cmsSetLogErrorHandler(lcms2ErrorFunc);

    PhotoStage::MainWindow w;
    w.show();

    return a.exec();
}