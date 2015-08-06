#-------------------------------------------------
#
# Project created by QtCreator 2015-05-04T21:50:45
#
#-------------------------------------------------

QT       += core gui sql positioning network

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
    cartography/map.cpp \
    cartography/photomarker.cpp \
    database/databaseaccess.cpp \
    database/dbutils.cpp \
    database/importinfo.cpp \
    database/importoptions.cpp \
    develop/develop.cpp \
    develop/modules/basicmodule.cpp \
    develop/modules/develophistogrammodule.cpp \
    develop/modules/developmodule.cpp \
    develop/modules/rawmodule.cpp \
    engine/colortransform.cpp \
    engine/engineutils.cpp \
    engine/operation.cpp \
    engine/pipelinebuilder.cpp \
    engine/platform.cpp \
    filmstriptile.cpp \
    image.cpp \
    import/exiv2lib.cpp \
    import/exivfacade.cpp \
    import/imagefilesystemmodel.cpp \
    import/imagefiletile.cpp \
    import/importbackgroundtask.cpp \
    import/importdialog.cpp \
    import/previewfileloader.cpp \
    import/previewinfo.cpp \
    jobs/colortransformjob.cpp \
    jobs/previewcacheloaderjob.cpp \
    jobs/previewgeneratorjob.cpp \
    library/imagedbtile.cpp \
    library/library.cpp \
    library/modules/collectionmodule.cpp \
    library/modules/filtermodule.cpp \
    library/modules/libraryhistogrammodule.cpp \
    library/modules/librarymodule.cpp \
    library/modules/metadatamodule.cpp \
    library/modules/shortcutmodule.cpp \
    library/modules/taggingmodule.cpp \
    main.cpp \
    mainwindow.cpp \
    photo.cpp \
    photomodel.cpp \
    photosortfilterproxymodel.cpp \
    preferences.cpp \
    preferencesdialog.cpp \
    previewcache.cpp \
    regenhashestask.cpp \
    runnable.cpp \
    sqlkeywordmodel.cpp \
    threadqueue.cpp \
    timeadjustdialog.cpp \
    utils.cpp \
    widgets/abstracttile.cpp \
    widgets/actionstategroup.cpp \
    widgets/backgroundtaskprogress.cpp \
    widgets/fixedtreeview.cpp \
    widgets/histogram.cpp \
    widgets/loupeview.cpp \
    widgets/mapview/abstractmarker.cpp \
    widgets/mapview/goolemapprovider.cpp \
    widgets/mapview/groupmarker.cpp \
    widgets/mapview/imagemarker.cpp \
    widgets/mapview/layer.cpp \
    widgets/mapview/mapprovider.cpp \
    widgets/mapview/mapview.cpp \
    widgets/mapview/openstreetmapmapprovider.cpp \
    widgets/modulepanel.cpp \
    widgets/tileview.cpp \
    widgets/translucentwindow.cpp \
    database/pathdao.cpp \
    database/photodao.cpp \
    database/pathitem.cpp \
    library/modules/filesmodule.cpp \
    dragdropinfo.cpp \
    library/modules/collectionmodel.cpp \
    library/modules/pathmodel.cpp \
    photoprivate.cpp \
    database/collectiondao.cpp \
    database/collectionitem.cpp

    #processing/amaze_demosaic_RT.c

HEADERS  += \
    aboutdialog.h \
    backgroundtask.h \
    backgroundtaskmanager.h \
    cartography/map.h \
    cartography/photomarker.h \
    constants.h \
    database/databaseaccess.h \
    database/dbutils.h \
    database/importinfo.h \
    database/importoptions.h \
    develop/develop.h \
    develop/modules/basicmodule.h \
    develop/modules/develophistogrammodule.h \
    develop/modules/developmodule.h \
    develop/modules/rawmodule.h \
    engine/colortransform.h \
    engine/engineutils.h \
    engine/operation.h \
    engine/pipelinebuilder.h \
    engine/platform.h \
    engine/platform_mac.h \
    filmstriptile.h \
    image.h \
    import/exiv2lib.h \
    import/exivfacade.h \
    import/exivutils.h \
    import/imagefilesystemmodel.h \
    import/imagefiletile.h \
    import/importbackgroundtask.h \
    import/importdialog.h \
    import/previewfileloader.h \
    import/previewinfo.h \
    jobs/colortransformjob.h \
    jobs/previewcacheloaderjob.h \
    jobs/previewgeneratorjob.h \
    library/imagedbtile.h \
    library/library.h \
    library/modules/collectionmodule.h \
    library/modules/filtermodule.h \
    library/modules/libraryhistogrammodule.h \
    library/modules/librarymodule.h \
    library/modules/metadatamodule.h \
    library/modules/shortcutmodule.h \
    library/modules/taggingmodule.h \
    mainwindow.h \
    module.h \
    nullable.h \
    photo.h \
    photofilterinfo.h \
    photomodel.h \
    photosortfilterproxymodel.h \
    preferences.h \
    preferencesdialog.h \
    previewcache.h \
    regenhashestask.h \
    runnable.h \
    sqlkeywordmodel.h \
    threadqueue.h \
    timeadjustdialog.h \
    utils.h \
    widgets/abstracttile.h \
    widgets/actionstategroup.h \
    widgets/backgroundtaskprogress.h \
    widgets/fixedtreeview.h \
    widgets/histogram.h \
    widgets/loupeview.h \
    widgets/mapview/abstractmarker.h \
    widgets/mapview/goolemapprovider.h \
    widgets/mapview/groupmarker.h \
    widgets/mapview/imagemarker.h \
    widgets/mapview/layer.h \
    widgets/mapview/mapprovider.h \
    widgets/mapview/mapview.h \
    widgets/mapview/openstreetmapmapprovider.h \
    widgets/modulepanel.h \
    widgets/tileview.h \
    widgets/translucentwindow.h \
    database/pathdao.h \
    database/photodao.h \
    database/pathitem.h \
    library/modules/filesmodule.h \
    dragdropinfo.h \
    library/modules/collectionmodel.h \
    library/modules/pathmodel.h \
    database/collectiondao.h \
    database/collectionitem.h

FORMS    += \
    mainwindow.ui \
    aboutdialog.ui \
    develop/develop.ui \
    develop/modules/basicmodule.ui \
    develop/modules/rawmodule.ui \
    import/importdialog.ui \
    library/library.ui \
    cartography/map.ui \
    preferencesdialog.ui \
    timeadjustdialog.ui \
    widgets/backgroundtaskprogress.ui \
    library/modules/metadatamodule.ui \
    library/modules/filtermodule.ui

DISTFILES += \
    Info.plist

include($$PWD/external/rawspeed/rawspeed.pri)
include($$PWD/external/xxHash/xxhash.pri)


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

