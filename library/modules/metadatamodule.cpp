#include "metadatamodule.h"
#include "ui_metadatamodule.h"

namespace PhotoStage
{
MetaDataModule::MetaDataModule(QWidget* parent) :
    LibraryModule(parent),
    ui(new Ui::MetaDataModule)
{
    ui->setupUi(this);
}

void MetaDataModule::setPhotos(const QList<Photo>& list)
{
    if (list.size() > 0)
    {
        Photo p = list.at(0);
        setMetaData(p);
    }
}

void MetaDataModule::setMetaData(const Photo& p)
{
    ExifInfo ei = p.exifInfo();

    ui->leAperture->setText("ƒ / "+QString::number(ei.aperture));
    ui->leArtist->setText(ei.artist);
    ui->leCameraMake->setText(ei.make);
    ui->leCameraModel->setText(ei.model);
    ui->leCopyright->setText(ei.copyright);
    ui->leDateTime->setText(ei.dateTimeDigitized.toString());
    ui->leDateTimeOriginal->setText(ei.dateTimeOriginal.toString());
    ui->leExposure->setText("⅟ "+QString::number((int)(1.0f/ei.exposureTime))+"s");
    ui->leFlash->setText(ei.flash ? "Fired" : "Didn't fire");
    ui->leFocalLength->setText(QString::number(ei.focalLength)+"mm");
    ui->leISO->setText(QString::number(ei.isoSpeed));
    ui->leLens->setText(ei.lensName);
}
}
