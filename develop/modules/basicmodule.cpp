#include <QDebug>

#include "basicmodule.h"
#include "ui_BasicModule.h"

BasicModule::BasicModule(QWidget* parent) : DevelopModule(parent), ui(new Ui::BasicModule)
{
    ui->setupUi(this);
}

BasicModule::~BasicModule()
{
    delete ui;
}

void BasicModule::onExposureChanged(int value)
{
    qDebug() << "exposure value" << value;

    if (mPhoto == NULL)
    {
        qDebug() << "Photo is NULL";
        return;
    }

    QImage img = mPhoto->original();

    float  mult = 1.0+value/200.0;

    qDebug() << "Multiplier" << mult;

    int count = img.bytesPerLine();

    for (int y = 0; y < img.height(); y++)
    {
        uint8_t* line = img.scanLine(y);

        for (int x = 0; x < count - 4; x += 4)
        {
            line[x]     = (uint8_t)((float)line[x] * mult);
            line[x + 1] = (uint8_t)((float)line[x + 1] * mult);
            line[x + 2] = (uint8_t)((float)line[x + 2] * mult);
        }
    }
    mPhoto->setLibraryPreview(img);
    emit parametersAdjusted();
}
