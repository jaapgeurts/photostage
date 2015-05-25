#-------------------------------------------------
#
# Project created by QtCreator 2015-05-04T21:50:45
#
#-------------------------------------------------

QT       += core gui sql concurrent

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
    sqlpathmodel.cpp \
    sqlkeywordmodel.cpp \
    aboutdialog.cpp \
    timeadjustdialog.cpp \
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
    library/library.cpp \
    develop/develop.cpp \
    map/map.cpp \
    widgets/backgroundtaskprogress.cpp \
    backgroundtaskmanager.cpp \
    backgroundtask.cpp \
    import/importbackgroundtask.cpp \
    workunits/importoptions.cpp \
    photo.cpp \
    photomodel.cpp

HEADERS  += mainwindow.h \
    constants.h \
    imagefileloader.h \
    preferencesdialog.h \
    databaseaccess.h \
    sqlpathmodel.h \
    sqlkeywordmodel.h \
    aboutdialog.h \
    timeadjustdialog.h \
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
    module.h \
    develop/develop.h \
    map/map.h \
    widgets/backgroundtaskprogress.h \
    backgroundtaskmanager.h \
    backgroundtask.h \
    import/importbackgroundtask.h \
    workunits/importoptions.h \
    photo.h \
    photomodel.h


FORMS    += mainwindow.ui \
    preferencesdialog.ui \
    aboutdialog.ui \
    timeadjustdialog.ui \
    import/importdialog.ui \
    library/library.ui \
    develop/develop.ui \
    map/map.ui \
    widgets/backgroundtaskprogress.ui

DISTFILES += \
    Info.plist


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

