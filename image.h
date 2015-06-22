#ifndef IMAGE_H
#define IMAGE_H

#include <QMetaType>
#include <QImage>
#include <QString>
#include <QFile>
#include <QSharedPointer>

const int IMG_NO_CHANNELS = 3;

class Image
{
    public:

        Image();
        Image(int width, int height);
        Image(const QSize& size);
        ~Image();

        Image clone() const;

        // static convenience functions
        static Image fromFile(const QString& filename);
        static Image fromQImage(const QImage& image);

        QImage toQImage() const;
        bool saveToFile(const QString& filename) const;

        // getters
        int width() const;
        int height() const;
        QSize size() const;

        float* data() const;
        float* scanLine(int l) const;

        bool isNull() const;

    private:

        QSharedPointer<float> mPixels;
        int    mWidth;
        int    mHeight;

};

Q_DECLARE_METATYPE(Image)
//Q_DECLARE_METATYPE(Image*)

#endif // IMAGE_H
