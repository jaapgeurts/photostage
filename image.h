#ifndef PHOTOSTAGE_IMAGE_H
#define PHOTOSTAGE_IMAGE_H

#include <QMetaType>
#include <QImage>
#include <QString>
#include <QFile>
#include <QSharedPointer>

#include "engine/engineutils.h"

namespace PhotoStage
{
const int IMG_DEPTH = 3;

class Image
{
    class ImagePrivate;

    public:

        enum Rotation
        {
            DontRotate       = 1, // No rotation Needed
            FlipY            = 2, // flip over y
            Rotate180        = 3, // rotate 180 (or flip x flip y)
            FlipX            = 4, // flip over x
            Rotate90CWFlipY  = 5, // rotate 90 CW & flip over y
            Rotate90CW       = 6, // rotate 90 CW
            Rotate90CCWFlipY = 7, // rotate 90 CCW flip over y
            Rotate90CCW      = 8 // rotate 90 CCW
        };

        Image();
        // Image will take ownership of the datapointer
        Image(int width, int height, uint16_t* data = nullptr);
        Image(int width, int height, int depth, uint8_t* data, Rotation rotate = DontRotate);
        Image(const QSize& size);
        Image(const QImage& image);
        ~Image();

        QImage toQImage() const;
        QImage toPreviewImage() const;

        // static convenience functions
        // static Image fromFile(const QString& filename);

        bool saveToFile(const QString& filename) const;

        // getters
        int width() const;
        int height() const;
        int depth() const;
        QSize size() const;

        uint16_t* data() const;
        uint16_t* scanLine(int l) const;

        bool isNull() const;

    private:

        QSharedPointer<ImagePrivate> d;

        class ImagePrivate
        {
            friend class Image;

            public:

                ImagePrivate();
                ImagePrivate(int width, int height, uint16_t* data = nullptr);
                ImagePrivate(int width, int height, int depth, uint8_t* data, Rotation rotate = DontRotate);
                ImagePrivate(const QSize& size);
                ImagePrivate(const QImage& image);
                ~ImagePrivate();

            private:

                uint16_t* mPixels;
                int       mWidth;
                int       mHeight;

                QImage toQImage() const;

                void copy8bitBufferToPlanar(const uint8_t* src, int depth, Rotation rotate);
        };
};
}

Q_DECLARE_METATYPE(PhotoStage::Image)

#endif // PHOTOSTAGE_IMAGE_H
