#ifndef EXIVFACADE_H
#define EXIVFACADE_H

#include <QImage>
#include <QDateTime>
#include <QGeoCoordinate>

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
    QString make;
    QString model;

    // Image details
    Rotation rotation;
    QGeoCoordinate location;
    QDateTime dateTimeOriginal;
    QDateTime dateTimeDigitized;
    QString copyright;
    QString artist;

    // photo tech details
    float exposureTime;
    float aperture;
    int isoSpeed;
    bool flash;
    float rgbCoeffients[3]; // white balance coefficients

    // Lens details
    QString lensName;
    float focalLength;
};

class ExivFacade
{
    public:

        static ExivFacade* createExivReader();

        virtual ~ExivFacade()
        {
        };

        virtual bool openFile(const QString& path) = 0;
        virtual QImage getPreview()                = 0;
        virtual ExifInfo data()                    = 0;

    protected:

        ExivFacade();
};
}

#endif // EXIVFACADE_H
