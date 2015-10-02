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
               $$PWD/external/halide/include \
               $$PWD/external/libjpeg-turbo

# Qt uses libstdc++
CONFIG += c++11

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

QMAKE_LFLAGS_RELEASE -= -O1

SOURCES += \
    aboutdialog.cpp \
    backgroundtask.cpp \
    backgroundtaskmanager.cpp \
    cartography/cartography.cpp \
    cartography/photomarker.cpp \
    database/collectiondao.cpp \
    database/collectionitem.cpp \
    database/databaseaccess.cpp \
    database/dbutils.cpp \
    database/importinfo.cpp \
    database/importoptions.cpp \
    database/keyworddao.cpp \
    database/keyworditem.cpp \
    database/pathdao.cpp \
    database/pathitem.cpp \
    database/photodao.cpp \
    develop/develop.cpp \
    develop/modules/basicmodule.cpp \
    develop/modules/develophistogrammodule.cpp \
    develop/modules/developmodule.cpp \
    develop/modules/rawmodule.cpp \
    dragdropinfo.cpp \
    engine/basicoperation.cpp \
    engine/colortransform.cpp \
    engine/dimensionoperation.cpp \
    engine/engineutils.cpp \
    engine/operation.cpp \
    engine/pipelinebuilder.cpp \
    engine/platform.cpp \
    filmstriptile.cpp \
    image.cpp \
    imageprivate.cpp \
    import/exiv2lib.cpp \
    import/exivfacade.cpp \
    import/imagefilesystemmodel.cpp \
    import/imagefiletile.cpp \
    import/importbackgroundtask.cpp \
    import/importdialog.cpp \
    import/previewfileloader.cpp \
    import/previewinfo.cpp \
    io/jpegio.cpp \
    io/pngio.cpp \
    io/rawio.cpp \
    jobs/colortransformjob.cpp \
    jobs/imageloaderjob.cpp \
    library/imagedbtile.cpp \
    library/library.cpp \
    library/modules/collectionmodel.cpp \
    library/modules/collectionmodule.cpp \
    library/modules/filesmodule.cpp \
    library/modules/filtermodule.cpp \
    library/modules/keywordmodel.cpp \
    library/modules/keywordmodule.cpp \
    library/modules/libraryhistogrammodule.cpp \
    library/modules/librarymodule.cpp \
    library/modules/metadatamodule.cpp \
    library/modules/pathmodel.cpp \
    library/modules/shortcutmodule.cpp \
    library/modules/taggingmodule.cpp \
    main.cpp \
    mainwindow.cpp \
    photo.cpp \
    photomodel.cpp \
    photoprivate.cpp \
    photosortfilterproxymodel.cpp \
    preferences.cpp \
    preferencesdialog.cpp \
    previewcache.cpp \
    regenhashestask.cpp \
    runnable.cpp \
    threadqueue.cpp \
    timeadjustdialog.cpp \
    utils.cpp \
    widgets/abstracttile.cpp \
    widgets/actionstategroup.cpp \
    widgets/backgroundtaskprogress.cpp \
    widgets/developview.cpp \
    widgets/fixedlistview.cpp \
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
    widgets/mapview/modelindexlayer.cpp \
    widgets/mapview/openstreetmapmapprovider.cpp \
    widgets/modulepanel.cpp \
    widgets/tileview.cpp \
    widgets/timezonepicker.cpp \
    widgets/translucentwindow.cpp \
    database/developsettingdao.cpp \
    database/developitems.cpp \
    develop/modules/historymodule.cpp \
    develop/modules/historymodel.cpp \
    engine/developrawparameters.cpp

#processing/amaze_demosaic_RT.c

HEADERS  += \
    aboutdialog.h \
    backgroundtask.h \
    backgroundtaskmanager.h \
    cartography/cartography.h \
    cartography/photomarker.h \
    constants.h \
    database/collectiondao.h \
    database/collectionitem.h \
    database/databaseaccess.h \
    database/dbutils.h \
    database/importinfo.h \
    database/importoptions.h \
    database/keyworddao.h \
    database/keyworditem.h \
    database/pathdao.h \
    database/pathitem.h \
    database/photodao.h \
    develop/develop.h \
    develop/modules/basicmodule.h \
    develop/modules/develophistogrammodule.h \
    develop/modules/developmodule.h \
    develop/modules/rawmodule.h \
    dragdropinfo.h \
    engine/basicoperation.h \
    engine/colortransform.h \
    engine/dimensionoperation.h \
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
    io/jpegio.h \
    io/pngio.h \
    io/rawio.h \
    jobs/colortransformjob.h \
    jobs/imageloaderjob.h \
    library/imagedbtile.h \
    library/library.h \
    library/modules/collectionmodel.h \
    library/modules/collectionmodule.h \
    library/modules/filesmodule.h \
    library/modules/filtermodule.h \
    library/modules/keywordmodel.h \
    library/modules/keywordmodule.h \
    library/modules/libraryhistogrammodule.h \
    library/modules/librarymodule.h \
    library/modules/metadatamodule.h \
    library/modules/pathmodel.h \
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
    threadqueue.h \
    timeadjustdialog.h \
    utils.h \
    widgets/abstracttile.h \
    widgets/actionstategroup.h \
    widgets/backgroundtaskprogress.h \
    widgets/developview.h \
    widgets/fixedlistview.h \
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
    widgets/mapview/modelindexlayer.h \
    widgets/mapview/openstreetmapmapprovider.h \
    widgets/modulepanel.h \
    widgets/tileview.h \
    widgets/timezonepicker.h \
    widgets/translucentwindow.h \
    database/developsettingdao.h \
    database/developitems.h \
    engine/developrawparameters.h \
    photoowner.h \
    develop/modules/historymodule.h \
    develop/modules/historymodel.h

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
include($$PWD/external/iccjpeg/iccjpeg.pri)


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

    Resources.files = resources/camera_color_matrix.json resources/database_schema.sql external/rawspeed/data/cameras.xml
    Resources.path = Contents/Resources
    Profiles.files = resources/Profiles
    Profiles.path = Contents/Resources
    QMAKE_BUNDLE_DATA += Resources Profiles
}

win32 {
# LIBS +=
}

LIBS += \
            $$PWD/external/exiv2/lib/libexiv2.a \
            $$PWD/external/halide/lib/libHalide.a \
            $$PWD/external/libjpeg-turbo/libjpeg.a \
            $$PWD/external/libjpeg-turbo/libturbojpeg.a \
# for libexiv2
            -lexpat \
            -lz \
# end libexiv2
         #   -ljpeg \
            -lpng \
            -llcms2

RESOURCES += \
    qdarkstyle/style.qrc \
    resources.qrc

