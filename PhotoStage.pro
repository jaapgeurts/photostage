#-------------------------------------------------
#
# Project created by QtCreator 2015-05-04T21:50:45
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PhotoStage
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    importdialog.cpp \
    imagefilesystemmodel.cpp \
    imagefileloader.cpp \
    preferencesdialog.cpp \
    databaseaccess.cpp \
    importworkunit.cpp \
    importinfo.cpp \
    sqlphotomodel.cpp \
    sqlpathmodel.cpp \
    sqlkeywordmodel.cpp \
    widgets/modulepanel.cpp \
    aboutdialog.cpp \
    widgets/fixedtreeview.cpp \
    librarymodules/keywordingmodule.cpp \
    timeadjustdialog.cpp \
    sqlphotoinfo.cpp \
    widgets/abstracttile.cpp \
    imagedbtile.cpp \
    imagefiletile.cpp \
    widgets/tileview.cpp \
    photoworkunit.cpp \
    librarymodules/librarymodule.cpp

HEADERS  += mainwindow.h \
    importdialog.h \
    imagefilesystemmodel.h \
    imagefileloader.h \
    preferencesdialog.h \
    databaseaccess.h \
    importworkunit.h \
    importinfo.h \
    sqlphotomodel.h \
    sqlpathmodel.h \
    sqlkeywordmodel.h \
    widgets/modulepanel.h \
    aboutdialog.h \
    widgets/fixedtreeview.h \
    librarymodules/keywordingmodule.h \
    timeadjustdialog.h \
    sqlphotoinfo.h \
    widgets/abstracttile.h \
    imagedbtile.h \
    imagefiletile.h \
    widgets/tileview.h \
    photoworkunit.h \
    librarymodules/librarymodule.h

FORMS    += mainwindow.ui \
    importdialog.ui \
    preferencesdialog.ui \
    aboutdialog.ui \
    timeadjustdialog.ui

DISTFILES +=


mac {
    #QMAKE_INFO_PLIST = Info.plist
    ICON = resources/appicon.icns
    HEADERS +=
    OBJECTIVE_SOURCES +=
    LIBS += \
            -framework AppKit
}

RESOURCES += \
    qdarkstyle/style.qrc \
    resources.qrc

