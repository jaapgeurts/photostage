#-------------------------------------------------
#
# Project created by QtCreator 2015-05-04T21:50:45
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PhotoStage
TEMPLATE =

DEPENDPATH += .
INCLUDEPATH += . \
               $$PWD/external/exiv2/include

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
    photomodel.cpp \
    preferences.cpp \
    library/modules/collectionmodule.cpp \
    library/modules/shortcutmodule.cpp \
    widgets/histogram.cpp \
    library/modules/taggingmodule.cpp \
    photodata.cpp \
    widgets/loupeview.cpp \
    develop/modules/rawmodule.cpp \
    library/modules/libraryhistogrammodule.cpp \
    develop/modules/develophistogrammodule.cpp \
    develop/modules/developmodule.cpp \
    develop/modules/basicmodule.cpp \
    filmstriptile.cpp \
    previewcache.cpp \
    engine/colortransform.cpp
    #processing/amaze_demosaic_RT.c

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
    photomodel.h \
    preferences.h \
    library/modules/collectionmodule.h \
    library/modules/shortcutmodule.h \
    widgets/histogram.h \
    library/modules/taggingmodule.h \
    photodata.h \
    widgets/loupeview.h \
    develop/modules/rawmodule.h \
    library/modules/libraryhistogrammodule.h \
    develop/modules/develophistogrammodule.h \
    develop/modules/developmodule.h \
    develop/modules/basicmodule.h \
    filmstriptile.h \
    previewcache.h \
    engine/colortransform.h


FORMS    += mainwindow.ui \
    preferencesdialog.ui \
    aboutdialog.ui \
    timeadjustdialog.ui \
    import/importdialog.ui \
    library/library.ui \
    develop/develop.ui \
    map/map.ui \
    widgets/backgroundtaskprogress.ui \
    develop/modules/rawmodule.ui \
    develop/modules/basicmodule.ui

DISTFILES += \
    Info.plist

include($$PWD/external/rawspeed/rawspeed.pri)


unix:!macx {
LIBS += \
     -liconv
}

macx {
    #QMAKE_INFO_PLIST = Info.plist
    QMAKE_CXXFLAGS += \
                    -std=c++11 \
                    -stdlib=libc++ \
                    -fms-extensions \
                    -Wignored-attributes
    ICON = resources/appicon.icns
    INCLUDEPATH += /opt/local/include
    OBJECTIVE_SOURCES +=
    LIBS += \
            -stdlib=libc++ \
            $$PWD/external/exiv2/lib/libexiv2.a \
            /usr/lib/libiconv.dylib \
# for release link to the dynamic lib
#            $$PWD/external/exiv2/lib/libexiv2.13.dylib \
            -L/opt/local/lib \
            -framework AppKit
}

win32 {
# LIBS +=
}

LIBS += \
# for libexiv2
            -lexpat \
            -lz \
# end libexiv2
            -ljpeg \
            -llcms2

RESOURCES += \
    qdarkstyle/style.qrc \
    resources.qrc

