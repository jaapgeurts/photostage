#include "rawmodule.h"
#include "ui_rawmodule.h"

#include "database/databaseaccess.h"
#include "engine/developrawparameters.h"

namespace PhotoStage
{
RawModule::RawModule(QWidget* parent) :
    DevelopModule(parent),
    ui(new Ui::RawModule)
{
    ui->setupUi(this);
}

void RawModule::setPhoto(Photo& photo)
{
    DevelopModule::setPhoto(photo);

    if (!mPhoto.isNull())
        setValues();
}

void RawModule::onRedChanged()
{
    float                                value = ui->sbRed->value();

    QSharedPointer<DevelopRawParameters> po     = DatabaseAccess::developSettingDao()->getRawSettings(mPhoto.id());
    QSharedPointer<DevelopRawParameters> params = QSharedPointer<DevelopRawParameters>(
        new DevelopRawParameters(-1, value / 1000.0, po->greenMultiplier, po->blueMultiplier));

    params->rotation = po->rotation;
    params->algorithm = po->algorithm;

    QString desc = QString("Red multiplier set to %1").arg(ui->sbRed->value());
    DatabaseAccess::developSettingDao()->insertRawSettings(mPhoto.id(), params, desc);

    QImage img;
    mPhoto.setDevelopPreviewsRGB(img);
    Image  img2;
    mPhoto.setOriginalImage(img2);

    emit parametersAdjusted();
}

void RawModule::onGreenChanged()
{
}

void RawModule::onBlueChanged()
{
}

void RawModule::setValues()
{
    QSharedPointer<DevelopRawParameters> params = DatabaseAccess::developSettingDao()->getRawSettings(mPhoto.id());

    if (!params.isNull())
    {
        ui->sbRed->setValue(params->redMultiplier * 1000);
        ui->sbGreen1->setValue(params->greenMultiplier * 1000);
        //ui->sbGreen2->setValue(params->greenMultiplier*1000);
        ui->sbGreen2->setEnabled(false);
        ui->sbBlue->setValue(params->blueMultiplier * 1000);
    }
}
}
