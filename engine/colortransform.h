#ifndef COLOR_H
#define COLOR_H

#include <lcms2.h>

#include <QImage>

const QString WORKING_COLOR_SPACE = "MelissaRGB";

class ColorTransform
{
    public:

        ColorTransform();
        ColorTransform(const QString& from, const QString& to);
        ~ColorTransform();

        bool isValid() const;

        QImage transformImage(const QImage& inImage) const;

    private:

        cmsHTRANSFORM mHTransform;
};

#endif // COLOR_H
