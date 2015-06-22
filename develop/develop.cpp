#include <QSettings>
#include <QDebug>

#include "develop.h"
#include "ui_develop.h"

#define SETTINGS_SPLITTER_DEVELOP_SIZES "developmodule/splitter_main"

namespace PhotoStage
{
    Develop::Develop(QWidget* parent) :
        Module(parent),
        ui(new Ui::Develop),
        mLoadPhoto(false)
    {
        ui->setupUi(this);

        QSettings  settings;
        QList<int> l;

        if (settings.contains(SETTINGS_SPLITTER_DEVELOP_SIZES))
        {
            foreach(QVariant v, settings.value(
                    SETTINGS_SPLITTER_DEVELOP_SIZES).toList())
            {
                l << v.toInt();
            }
        }
        else
        {
            l << 200 << 600 << 200;
        }
        ui->splitterDevelop->setSizes(l);

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
        connect(mBasicModule,
            &BasicModule::parametersAdjusted,
            this,
            &Develop::imageChanged);
    }

    Develop::~Develop()
    {
        QSettings    settings;
        QVariantList list;


        foreach(int size, ui->splitterDevelop->sizes())
        {
            list << size;
        }
        settings.setValue(SETTINGS_SPLITTER_DEVELOP_SIZES, list);

        delete ui;
    }

    QRect Develop::lightGap()
    {
        // TODO: implement this
    }

    void Develop::setPhoto(Photo* photo)
    {
        if (photo == NULL)
        {
            qDebug() << "Develop::setPhoto(Photo*) called with NULL argument";
            return;
        }
        // determine visibility

        mPhoto = photo;

        // if visible, load immediately
        if (isVisible())
            doSetPhoto(photo);
        else
            // TODO: if invisible, defer loading until visible
            mLoadPhoto = true;
    }

    void Develop::imageChanged()
    {
        ui->developView->update();
        mHistogramModule->setPhoto(mPhoto);
        //  mHistogramModule->recalculate();
    }

    void Develop::showEvent(QShowEvent*)
    {
        if (mLoadPhoto)
        {
            doSetPhoto(mPhoto);
            mLoadPhoto = false;
        }
    }

    void Develop::doSetPhoto(Photo* photo)
    {
        ui->developView->setPhoto(photo);
        mHistogramModule->setPhoto(photo);
        mRawModule->setPhoto(photo);
        mBasicModule->setPhoto(photo);
    }
}