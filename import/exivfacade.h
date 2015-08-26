#ifndef PHOTOSTAGE_EXIVFACADE_H
#define PHOTOSTAGE_EXIVFACADE_H

#include <QImage>
#include <QDateTime>
#include <QGeoCoordinate>

#include "nullable.h"

namespace PhotoStage
{
struct ExifInfo
{
    enum Rotation
    {
        NotRotated       = 1, // No rotation Needed
        FlipY            = 2, // flip over y
        Rotate180        = 3, // rotate 180 (or flip x flip y)
        FlipX            = 4, // flip over x
        Rotate90CWFlipY  = 5, // rotate 90 CW & flip over y
        Rotate90CW       = 6, // rotate 90 CW
        Rotate90CCWFlipY = 7, // rotate 90 CCW flip over y
        Rotate90CCW      = 8  // rotate 90 CCW
    };

    // Camera tech details
    Nullable<QString> make;
    Nullable<QString> model;

    // Image details
    Rotation                 rotation;
    Nullable<QGeoCoordinate> location;
    Nullable<QDateTime>      dateTimeOriginal;
    Nullable<QDateTime>      dateTimeDigitized;
    Nullable<QString>        copyright;
    Nullable<QString>        artist;

    int width;
    int height;

    // photo tech details
    Nullable<float>   exposureTime;
    Nullable<float>   aperture;
    Nullable<uint8_t> isoSpeed;
    Nullable<bool>    flash;
    float             rgbCoeffients[3]; // white balance coefficients

    // Lens details
    Nullable<QString> lensName;
    Nullable<float>   focalLength;

    QString formatExposure() const;
    QString formatDimension() const;
};

class ExivFacade
{
    public:

        static ExivFacade* createExivReader();

        virtual ~ExivFacade()
        {
        };

        virtual bool openFile(const QString& path)    = 0;
        virtual bool openData(const QByteArray& data) = 0;
        virtual QImage getPreview()                   = 0;
        virtual ExifInfo data()                       = 0;

    protected:

        ExivFacade();
};
}

#endif // PHOTOSTAGE_EXIVFACADE_H
