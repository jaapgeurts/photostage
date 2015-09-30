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

void RawModule::setValues()
{
    DevelopRawParameters params = DatabaseAccess::developSettingDao()->getRawSettings(mPhoto.id());

    if (!params.isNull())
    {
        ui->sbRed->setValue(params.redMultiplier() * 1000);
        ui->sbGreen1->setValue(params.greenMultiplier() * 1000);
        //ui->sbGreen2->setValue(params.greenMultiplier()*1000);
        ui->sbGreen2->setEnabled(false);
        ui->sbBlue->setValue(params.blueMultiplier() * 1000);
    }
}
}
