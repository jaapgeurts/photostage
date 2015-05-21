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
    imagefileloader.cpp \
    preferencesdialog.cpp \
    databaseaccess.cpp \
    sqlphotomodel.cpp \
    sqlpathmodel.cpp \
    sqlkeywordmodel.cpp \
    aboutdialog.cpp \
    timeadjustdialog.cpp \
    sqlphotoinfo.cpp \
    workunits/photoworkunit.cpp \
    workunits/importinfo.cpp \
    workunits/importworkunit.cpp \
    library/imagedbtile.cpp \
    library/modules/keywordingmodule.cpp \
    library/modules/librarymodule.cpp \
    widgets/modulepanel.cpp \
    widgets/abstracttile.cpp \
    widgets/fixedtreeview.cpp \
    widgets/tileview.cpp \
    widgets/translucentwindow.cpp \
    import/importdialog.cpp \
    import/imagefiletile.cpp \
    import/previewfileloader.cpp \
    import/exivfacade.cpp \
    import/exiv2lib.cpp \
    import/imagefilesystemmodel.cpp \
    import/previewinfo.cpp \
    library/library.cpp

HEADERS  += mainwindow.h \
    constants.h \
    imagefileloader.h \
    preferencesdialog.h \
    databaseaccess.h \
    sqlphotomodel.h \
    sqlpathmodel.h \
    sqlkeywordmodel.h \
    aboutdialog.h \
    timeadjustdialog.h \
    sqlphotoinfo.h \
    workunits/photoworkunit.h \
    workunits/importinfo.h \
    workunits/importworkunit.h \
    library/imagedbtile.h \
    library/modules/keywordingmodule.h \
    library/modules/librarymodule.h \
    widgets/modulepanel.h \
    widgets/fixedtreeview.h \
    widgets/abstracttile.h \
    widgets/tileview.h \
    widgets/translucentwindow.h \
    import/importdialog.h \
    import/imagefilesystemmodel.h \
    import/imagefiletile.h \
    import/previewfileloader.h \
    import/exivfacade.h \
    import/exiv2lib.h \
    import/previewinfo.h \
    library/library.h \
    module.h


FORMS    += mainwindow.ui \
    preferencesdialog.ui \
    aboutdialog.ui \
    timeadjustdialog.ui \
    import/importdialog.ui \
    library/library.ui

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

