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

    if (ei.exposureTime != nullptr)
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
}
}
