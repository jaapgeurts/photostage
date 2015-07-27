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
    const ExifInfo& ei = ((Photo&)p).exifInfo();

    if (ei.aperture != nullptr)
        ui->leAperture->setText("ƒ / " + QString::number(ei.aperture.value));

    if (ei.artist != nullptr)
        ui->leArtist->setText(ei.artist.value);

    if (ei.make != nullptr)
        ui->leCameraMake->setText(ei.make.value);

    if (ei.model != nullptr)
        ui->leCameraModel->setText(ei.model.value);

    if (ei.copyright != nullptr)
        ui->leCopyright->setText(ei.copyright.value);

    if (ei.dateTimeDigitized != nullptr)
        ui->leDateTime->setText(ei.dateTimeDigitized.value.toString());

    if (ei.dateTimeOriginal != nullptr)
        ui->leDateTimeOriginal->setText(ei.dateTimeOriginal.value.toString());

    if (ei.exposureTime != nullptr)
        ui->leExposure->setText("⅟ " + QString::number((int)(1.0f / ei.exposureTime.value)) + "s");

    if (ei.flash != nullptr)
        ui->leFlash->setText(ei.flash.value ? "Fired" : "Didn't fire");

    if (ei.focalLength != nullptr)
        ui->leFocalLength->setText(QString::number(ei.focalLength.value) + "mm");

    if (ei.isoSpeed != nullptr)
        ui->leISO->setText(QString::number(ei.isoSpeed.value));

    if (ei.lensName != nullptr)
        ui->leLens->setText(ei.lensName.value);
    ui->leDimensions->setText(ei.formatDimension());

    if (ei.location != nullptr)
        ui->leGPS->setText(ei.location.value.toString(QGeoCoordinate::DegreesMinutesWithHemisphere));
}
}
