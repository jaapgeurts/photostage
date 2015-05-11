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
    qphotomodel.cpp \
    cellflowview.cpp \
    abstractcellrenderer.cpp \
    imagefilecellrenderer.cpp \
    imagefilesystemmodel.cpp \
    imagefileloader.cpp \
    preferencesdialog.cpp \
    databaseaccess.cpp \
    importworkunit.cpp \
    importinfo.cpp \
    volumediscoverer.cpp \
    volumediscovererimpl.cpp

HEADERS  += mainwindow.h \
    importdialog.h \
    qphotomodel.h \
    cellflowview.h \
    abstractcellrenderer.h \
    imagefilecellrenderer.h \
    imagefilesystemmodel.h \
    imagefileloader.h \
    preferencesdialog.h \
    databaseaccess.h \
    importworkunit.h \
    importinfo.h \
    volumediscoverer.h \
    volumediscovererimpl.h \
    volumediscovererplatform.h

FORMS    += mainwindow.ui \
    importdialog.ui \
    preferencesdialog.ui

DISTFILES +=

mac {
    HEADERS +=  \
            volumediscoverermac.h \
            cocoainitializer.h
    OBJECTIVE_SOURCES += \
            volumediscoverermac.mm \
            cocoainitializer.mm
    LIBS += \
            -framework AppKit
}

