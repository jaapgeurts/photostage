#include <QSettings>
#include <QDebug>

#include "database/databaseaccess.h"
#include "engine/colortransform.h"
#include "jobs/imageloaderjob.h"
#include "constants.h"
#include "develop.h"
#include "ui_develop.h"

namespace PhotoStage
{
Develop::Develop( QWidget* parent) :
    Module(parent),
    ui(new Ui::Develop),
    mLoadPhoto(false)
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

    // Basic
    mBasicModule = new BasicModule(ui->DevelopPanel);
    ui->DevelopPanel->addPanel("Basic", mBasicModule);
    connect(mBasicModule, &BasicModule::parametersAdjusted, this, &Develop::onDevelopSettingsChanged);

    // HistoryModule
    mHistoryModule = new HistoryModule(ui->HistoryPanel);
    ui->HistoryPanel->addPanel("History", mHistoryModule);
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

    delete ui;
}

QRect Develop::lightGap()
{
    // TODO: implement this
    return QRect();
}

void Develop::setPhoto(Photo photo)
{
    qDebug () << "DEVELOP: setting photo";
    mPhoto = photo;

    if (mPhoto.isNull())
        return;

    // load develop history
    //   mDevelopHistory = DatabaseAccess::developSettingDao()->getDevelopHistory(photo);

    // if visible, load immediately
    if (isVisible())
        doSetPhoto(mPhoto);
    else
        // TODO: if invisible, defer loading until visible
        mLoadPhoto = true;
}

void Develop::onPhotoUpdated()
{
    if (!mPhoto.isNull())
    {
        qDebug() << "DEVELOP: Setting photo in view";
        ui->developView->setPhoto(mPhoto.developPreviewsRGB());
    }
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

void Develop::doSetPhoto(Photo photo)
{
    // check if the original photo is here. if not then load it from disk.
    if (photo.isNull())
        return;

    if (photo.isRaw())
    {
        if (!ui->DevelopPanel->containsPanel("RAW"))
        {
            // Raw module
            mRawModule = new RawModule(ui->DevelopPanel);
            connect(mRawModule, &BasicModule::parametersAdjusted, this, &Develop::onDevelopSettingsChanged);

            ui->DevelopPanel->addPanel("RAW", mRawModule);
        }
    }
    else
    {
        ui->DevelopPanel->removePanel("RAW");
        delete mRawModule;
        mRawModule = nullptr;
    }

    ui->developView->setPhoto(photo.developPreviewsRGB());

    mHistogramModule->setPhoto(photo);

    if (mRawModule != nullptr)
        mRawModule->setPhoto(photo);
    mBasicModule->setPhoto(photo);
    mHistoryModule->setPhoto(photo);
}
}
