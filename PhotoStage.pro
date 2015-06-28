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
               $$PWD/external/exiv2/include \
               $$PWD/external/halide/include

# Qt uses libstdc++
CONFIG += c++11

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

QMAKE_LFLAGS_RELEASE -= -O1

SOURCES += \
    aboutdialog.cpp \
    backgroundtask.cpp \
    backgroundtaskmanager.cpp \
    databaseaccess.cpp \
    develop/develop.cpp \
    develop/modules/basicmodule.cpp \
    develop/modules/develophistogrammodule.cpp \
    develop/modules/developmodule.cpp \
    develop/modules/rawmodule.cpp \
    engine/colortransform.cpp \
    engine/engineutils.cpp \
    engine/operation.cpp \
    engine/pipelinebuilder.cpp \
    filmstriptile.cpp \
    image.cpp \
    imagefileloader.cpp \
    import/exiv2lib.cpp \
    import/exivfacade.cpp \
    import/imagefilesystemmodel.cpp \
    import/imagefiletile.cpp \
    import/importbackgroundtask.cpp \
    import/importdialog.cpp \
    import/previewfileloader.cpp \
    import/previewinfo.cpp \
    library/imagedbtile.cpp \
    library/library.cpp \
    library/modules/collectionmodule.cpp \
    library/modules/libraryhistogrammodule.cpp \
    library/modules/librarymodule.cpp \
    library/modules/shortcutmodule.cpp \
    library/modules/taggingmodule.cpp \
    main.cpp\
    mainwindow.cpp \
    map/map.cpp \
    photo.cpp \
    photomodel.cpp \
    preferences.cpp \
    preferencesdialog.cpp \
    previewcache.cpp \
    sqlkeywordmodel.cpp \
    sqlpathmodel.cpp \
    timeadjustdialog.cpp \
    utils.cpp \
    widgets/abstracttile.cpp \
    widgets/backgroundtaskprogress.cpp \
    widgets/fixedtreeview.cpp \
    widgets/histogram.cpp \
    widgets/loupeview.cpp \
    widgets/modulepanel.cpp \
    widgets/tileview.cpp \
    widgets/translucentwindow.cpp \
    workunits/importinfo.cpp \
    workunits/importoptions.cpp \
    workunits/importworkunit.cpp \
    workunits/photoworkunit.cpp \
    engine/platform.cpp \
    widgets/actionstategroup.cpp

    #processing/amaze_demosaic_RT.c

HEADERS  += \
    aboutdialog.h \
    backgroundtask.h \
    backgroundtaskmanager.h \
    constants.h \
    databaseaccess.h \
    develop/develop.h \
    develop/modules/basicmodule.h \
    develop/modules/develophistogrammodule.h \
    develop/modules/developmodule.h \
    develop/modules/rawmodule.h \
    engine/colortransform.h \
    engine/engineutils.h \
    engine/operation.h \
    engine/pipelinebuilder.h \
    filmstriptile.h \
    image.h \
    imagefileloader.h \
    import/exiv2lib.h \
    import/exivfacade.h \
    import/imagefilesystemmodel.h \
    import/imagefiletile.h \
    import/importbackgroundtask.h \
    import/importdialog.h \
    import/previewfileloader.h \
    import/previewinfo.h \
    library/imagedbtile.h \
    library/library.h \
    library/modules/collectionmodule.h \
    library/modules/libraryhistogrammodule.h \
    library/modules/librarymodule.h \
    library/modules/shortcutmodule.h \
    library/modules/taggingmodule.h \
    mainwindow.h \
    map/map.h \
    module.h \
    photo.h \
    photodata.h \
    photomodel.h \
    preferences.h \
    preferencesdialog.h \
    previewcache.h \
    sqlkeywordmodel.h \
    sqlpathmodel.h \
    timeadjustdialog.h \
    utils.h \
    widgets/abstracttile.h \
    widgets/backgroundtaskprogress.h \
    widgets/fixedtreeview.h \
    widgets/histogram.h \
    widgets/loupeview.h \
    widgets/modulepanel.h \
    widgets/tileview.h \
    widgets/translucentwindow.h \
    workunits/importinfo.h \
    workunits/importoptions.h \
    workunits/importworkunit.h \
    workunits/photoworkunit.h \
    engine/platform_mac.h \
    engine/platform.h \
    widgets/actionstategroup.h

FORMS    += \
    mainwindow.ui \
    aboutdialog.ui \
    develop/develop.ui \
    develop/modules/basicmodule.ui \
    develop/modules/rawmodule.ui \
    import/importdialog.ui \
    library/library.ui \
    map/map.ui \
    preferencesdialog.ui \
    timeadjustdialog.ui \
    widgets/backgroundtaskprogress.ui

DISTFILES += \
    Info.plist

include($$PWD/external/rawspeed/rawspeed.pri)


unix:!macx {
LIBS += \
     -liconv
}

macx {
    #QMAKE_INFO_PLIST = Info.plist
#    QMAKE_CXXFLAGS -= -std=c++0x
    QMAKE_CXXFLAGS += \
                    -std=c++11 \
                    -stdlib=libc++ \
                    -fms-extensions \
                    -Wignored-attributes
    SOURCES += \
        engine/platform_mac.c
    ICON = resources/appicon.icns
    INCLUDEPATH += /opt/local/include
    OBJECTIVE_SOURCES +=
    LIBS += \
            -stdlib=libc++ \
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
            $$PWD/external/exiv2/lib/libexiv2.a \
            $$PWD/external/halide/lib/libHalide.a \
# for libexiv2
            -lexpat \
            -lz \
# end libexiv2
            -ljpeg \
            -lpng \
            -llcms2

RESOURCES += \
    qdarkstyle/style.qrc \
    resources.qrc

