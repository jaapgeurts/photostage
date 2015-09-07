#include <QSettings>
#include <QDebug>

#include "engine/colortransform.h"
#include "jobs/imageloaderjob.h"
#include "constants.h"
#include "develop.h"
#include "ui_develop.h"

namespace PhotoStage
{
Develop::Develop(QWidget* parent) :
    Module(parent),
    ui(new Ui::Develop),
    mLoadPhoto(false),
    mPhotoModel(NULL),
    mThreadQueue(new ThreadQueue())
{
    ui->setupUi(this);

    QSettings  settings;
    settings.beginGroup(SETTINGS_GROUP_DEVELOP);
    QList<int> l;

    if (settings.contains(SETTINGS_SPLITTER_DEVELOP_SIZES))
    {
        foreach(QVariant v, settings.value(SETTINGS_SPLITTER_DEVELOP_SIZES).toList())
        {
            l << v.toInt();
        }
    }
    else
    {
        l << 200 << 600 << 200;
    }
    ui->splitterDevelop->setSizes(l);
    settings.endGroup();

    // add panels for development.

    // Histogram
    mHistogramModule = new DevelopHistogramModule(ui->DevelopPanel);
    ui->DevelopPanel->addPanel("Histogram", mHistogramModule);

    // Raw
    mRawModule = new RawModule(ui->DevelopPanel);
    ui->DevelopPanel->addPanel("RAW", mRawModule);

    // Basic
    mBasicModule = new BasicModule(ui->DevelopPanel);
    ui->DevelopPanel->addPanel("Basic", mBasicModule);
    connect(mBasicModule, &BasicModule::parametersAdjusted, this, &Develop::onDevelopSettingsChanged);
}

Develop::~Develop()
{
    QSettings settings;

    settings.beginGroup(SETTINGS_GROUP_DEVELOP);

    QVariantList list;

    foreach(int size, ui->splitterDevelop->sizes())
    {
        list << size;
    }
    settings.setValue(SETTINGS_SPLITTER_DEVELOP_SIZES, list);
    settings.endGroup();

    delete mThreadQueue;

    delete ui;
}

QRect Develop::lightGap()
{
    // TODO: implement this
    return QRect();
}

void Develop::setPhoto(Photo photo)
{
    mPhoto = photo;

    // if visible, load immediately
    if (isVisible())
        doSetPhoto(mPhoto);
    else
        // TODO: if invisible, defer loading until visible
        mLoadPhoto = true;
}

void Develop::onPhotoUpdated()
{
    ui->developView->update();
}

void Develop::onDevelopSettingsChanged()
{
    if (mPhoto.isNull())
        return;

    ui->developView->setPhoto(mPhoto.developPreviewsRGB());
    //  ui->developView->update();
    mHistogramModule->setPhoto(mPhoto);
}

void Develop::showEvent(QShowEvent*)
{
    if (mLoadPhoto)
    {
        doSetPhoto(mPhoto);
        mLoadPhoto = false;
    }
}

void Develop::loadDevelopPreview()
{
    ImageLoaderJob* ilj = new ImageLoaderJob(mPhoto, false);

    ilj->connect(ilj, &ImageLoaderJob::imageReady, this, &Develop::onImageLoaded);
    uint32_t id = mThreadQueue->addJob(ilj);
}

void Develop::onImageLoaded(Photo photo, const Image& image)
{
    photo.setOriginalImage(image);
    //    ColorTransform tr1 = ColorTransform::getTransform("DevToQ", WORKING_COLOR_SPACE, "sRGB",
    //            ColorTransform::FORMAT_RGB48_PLANAR, ColorTransform::FORMAT_RGB32);
    //    QImage qimg = tr1.transformToQImage(image);
    QImage qimg = image.toQImage();
    photo.setDevelopPreviewsRGB(qimg);
    ui->developView->setPhoto(qimg);
}

void Develop::doSetPhoto(Photo photo)
{
    // check if the original photo is here. if not then load it from disk.
    if (photo.isNull())
        return;

    if (photo.developPreviewsRGB().isNull())
        loadDevelopPreview();
    else
        ui->developView->setPhoto(mPhoto.developPreviewsRGB());

    mHistogramModule->setPhoto(photo);
    mRawModule->setPhoto(photo);
    mBasicModule->setPhoto(photo);
}
}
