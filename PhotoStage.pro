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
    widgets/cellflowview.cpp \
    widgets/abstractcellrenderer.cpp \
    imagefilecellrenderer.cpp \
    imagefilesystemmodel.cpp \
    imagefileloader.cpp \
    preferencesdialog.cpp \
    databaseaccess.cpp \
    importworkunit.cpp \
    importinfo.cpp \
    sqlphotomodel.cpp \
    imagedbcellrenderer.cpp \
    sqlpathmodel.cpp \
    sqlkeywordmodel.cpp \
    widgets/modulepanel.cpp \
    aboutdialog.cpp \
    widgets/fixedtreeview.cpp \
    librarymodules/keywordingmodule.cpp \
    timeadjustdialog.cpp \
    sqlphotoinfo.cpp

HEADERS  += mainwindow.h \
    importdialog.h \
    widgets/cellflowview.h \
    widgets/abstractcellrenderer.h \
    imagefilecellrenderer.h \
    imagefilesystemmodel.h \
    imagefileloader.h \
    preferencesdialog.h \
    databaseaccess.h \
    importworkunit.h \
    importinfo.h \
    sqlphotomodel.h \
    imagedbcellrenderer.h \
    sqlpathmodel.h \
    sqlkeywordmodel.h \
    widgets/modulepanel.h \
    aboutdialog.h \
    widgets/fixedtreeview.h \
    librarymodules/keywordingmodule.h \
    timeadjustdialog.h \
    sqlphotoinfo.h

FORMS    += mainwindow.ui \
    importdialog.ui \
    preferencesdialog.ui \
    aboutdialog.ui \
    timeadjustdialog.ui

DISTFILES +=


mac {
    #QMAKE_INFO_PLIST = Info.plist
    ICON = appicon.icns
    HEADERS +=
    OBJECTIVE_SOURCES +=
    LIBS += \
            -framework AppKit
}

RESOURCES += \
    qdarkstyle/style.qrc \
    resources.qrc

