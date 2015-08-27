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

void MetaDataModule::setPhoto(Photo& photo)
{
    clearAll();

    if (!photo.isNull())
        setMetaData(photo);
}

void MetaDataModule::clearAll()
{
    ui->leAperture->clear();
    ui->leArtist->clear();
    ui->leCameraMake->clear();
    ui->leCameraModel->clear();
    ui->leCopyright->clear();
    ui->leDateTime->clear();
    ui->leDateTimeOriginal->clear();
    ui->leExposure->clear();
    ui->leFlash->clear();
    ui->leFocalLength->clear();
    ui->leISO->clear();
    ui->leLens->clear();
    ui->leGPS->clear();
    ui->leDimensions->clear();
    ui->leColorProfile->clear();
}

void MetaDataModule::setMetaData(const Photo& p)
{
    const ExifInfo& ei = ((Photo&)p).exifInfo();

    // FIXME: this crashes sometimes
    if (ei.aperture != nullptr)
        ui->leAperture->setText("ƒ / " + QString::number(*ei.aperture));

    if (ei.artist != nullptr)
        ui->leArtist->setText(*ei.artist);

    if (ei.make != nullptr)
        ui->leCameraMake->setText(*ei.make);

    if (ei.model != nullptr)
        ui->leCameraModel->setText(*ei.model);

    if (ei.copyright != nullptr)
        ui->leCopyright->setText(*ei.copyright);

    if (ei.dateTimeDigitized != nullptr)
        ui->leDateTime->setText(ei.dateTimeDigitized->toString());

    if (ei.dateTimeOriginal != nullptr)
        ui->leDateTimeOriginal->setText(ei.dateTimeOriginal->toString());

    if (ei.exposureTime !=  nullptr)
        ui->leExposure->setText("⅟ " + QString::number((int)(1.0f / *ei.exposureTime)) + "s");

    if (ei.flash != nullptr)
        ui->leFlash->setText(*ei.flash ? "Fired" : "Didn't fire");

    if (ei.focalLength != nullptr)
        ui->leFocalLength->setText(QString::number(*ei.focalLength) + "mm");

    if (ei.isoSpeed != nullptr)
        ui->leISO->setText(QString::number(*ei.isoSpeed));

    if (ei.lensName != nullptr)
        ui->leLens->setText(*ei.lensName);

    ui->leDimensions->setText(ei.formatDimension());

    if (ei.location != nullptr)
        ui->leGPS->setText(ei.location->toString(QGeoCoordinate::DegreesMinutesWithHemisphere));

    if (!ei.profileName.isNull())
        ui->leColorProfile->setText(ei.profileName);
}
}
