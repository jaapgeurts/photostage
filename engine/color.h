#ifndef COLOR_H
#define COLOR_H

#include <QImage>

#define WORKING_COLOR_SPACE "MelissaRGB"

QImage convertImageProfile(const QImage& inImage,const QString& from, const QString& to);

class Color
{
public:
    Color();
};

#endif // COLOR_H
