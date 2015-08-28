#ifndef PHOTOSTAGE_IMAGE_H
#define PHOTOSTAGE_IMAGE_H

#include <QMetaType>
#include <QImage>
#include <QString>
#include <QFile>
#include <QSharedPointer>

namespace PhotoStage
{
const int IMG_DEPTH = 3;

class Image
{
    class ImagePrivate;

    public:

        Image();
        Image(int width, int height);
        Image(const QSize& size);
        ~Image();

        // static convenience functions
        static Image fromFile(const QString& filename);
        static Image fromQImage(const QImage& image);

        QImage toQImage() const;
        bool saveToFile(const QString& filename) const;

        // getters
        int width() const;
        int height() const;
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
                ImagePrivate(int width, int height);
                ImagePrivate(const QSize& size);
                ~ImagePrivate();

            private:

                uint16_t* mPixels;
                int       mWidth;
                int       mHeight;

                static Image fromFilePNG(const QString& filename);
                bool saveToFilePNG(const QString& filename) const;
        };
};
}

Q_DECLARE_METATYPE(PhotoStage::Image)
//Q_DECLARE_METATYPE(Image*)

#endif // PHOTOSTAGE_IMAGE_H
