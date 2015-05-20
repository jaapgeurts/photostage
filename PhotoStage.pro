#-------------------------------------------------
#
# Project created by QtCreator 2015-05-04T21:50:45
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PhotoStage
TEMPLATE = app

# Qt uses libstdc++
CONFIG += c++11

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
    librarymodules/librarymodule.cpp \
    widgets/translucentwindow.cpp \
    previewfileloader.cpp \
    exivfacade.cpp \
    exiv2lib.cpp

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
    librarymodules/librarymodule.h \
    widgets/translucentwindow.h \
    previewfileloader.h \
    exivfacade.h \
    constants.h \
    exiv2lib.h

FORMS    += mainwindow.ui \
    importdialog.ui \
    preferencesdialog.ui \
    aboutdialog.ui \
    timeadjustdialog.ui

DISTFILES +=


mac {
    #QMAKE_INFO_PLIST = Info.plist
    QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
    ICON = resources/appicon.icns
    INCLUDEPATH += /opt/local/include
    HEADERS +=
    OBJECTIVE_SOURCES +=
    LIBS += \
            -stdlib=libc++ \
            -L/opt/local/lib \
            -lexiv2 \
            -framework AppKit
}

RESOURCES += \
    qdarkstyle/style.qrc \
    resources.qrc

