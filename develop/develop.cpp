#include <QSettings>

#include "develop.h"
#include "ui_develop.h"

#define SETTINGS_SPLITTER_DEVELOP_SIZES "developmodule/splitter_main"

Develop::Develop(QWidget *parent) :
    Module(parent),
    ui(new Ui::Develop)
{
    ui->setupUi(this);

    QSettings settings;
    QList<int> l;
    if (settings.contains(SETTINGS_SPLITTER_DEVELOP_SIZES))
    {
        foreach(QVariant v, settings.value(SETTINGS_SPLITTER_DEVELOP_SIZES).toList())
        {
            l << v.toInt();
        }
    } else
    {
        l << 200 << 600 << 200;
    }
    ui->splitterDevelop->setSizes(l);

    // add panels for development.

    // Histogram
    mHistogramModule = new DevelopHistogramModule(ui->DevelopPanel);
    ui->DevelopPanel->addPanel("Histogram",mHistogramModule);

    // Raw
    mRawModule = new RawModule(ui->DevelopPanel);
    ui->DevelopPanel->addPanel("RAW",mRawModule);

    // Normal
    mBasicModule = new BasicModule(ui->DevelopPanel);
    ui->DevelopPanel->addPanel("Basic",mBasicModule);


}

Develop::~Develop()
{
    QSettings settings;
    QVariantList list;
    foreach(int size, ui->splitterDevelop->sizes())
    {
        list << size;
    }
    settings.setValue(SETTINGS_SPLITTER_DEVELOP_SIZES,list);

    delete ui;
}

QRect Develop::lightGap()
{
// TODO: implement this
}
