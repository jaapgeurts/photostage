#ifndef PHOTOSTAGE_COLOR_H
#define PHOTOSTAGE_COLOR_H

#include <QImage>
#include <QHash>

#include <lcms2.h>

#include "image.h"

namespace PhotoStage
{
const QString WORKING_COLOR_SPACE = "MelissaRGB";

class ColorTransform
{
    public:

        enum Format
        {
            FORMAT_FLOAT,
            FORMAT_RGB32,     // 8bit channels (RGB channel + extra)
            FORMAT_GRAYSCALE8,
            FORMAT_RGB48     //  16 bit channels RRRR GGGG BBBB
        };

        // convenience functions that retain the result for future use
        static ColorTransform getTransform(const QString& iden,
            const QString& from, const QString& to, Format inFormat =  FORMAT_FLOAT, Format outFormat = FORMAT_FLOAT);
        static ColorTransform getTransform(const QByteArray& srcProfile,
            const QString& to, Format inFormat, Format outFormat);

        static QString getMonitorProfilePath();

        ColorTransform();
        ColorTransform(const QString& from, const QString& to,
            Format inFormat = FORMAT_FLOAT, Format outFormat = FORMAT_FLOAT);
        ColorTransform(const QByteArray& srcProfile, const QString& to, Format inFormat, Format outFormat);
        ColorTransform(const cmsHTRANSFORM& cmsTransform);
        ~ColorTransform();

        bool isValid() const;
        const QString& profileName();

        Image transformImage(const Image& inImage) const;
        QImage transformToQImage(const Image& inImage) const;
        QImage transformQImage(const QImage& inImage) const;
        Image transformFromQImage(const QImage& inImage) const;

    private:

        static QHash<QString, ColorTransform> mTransformCache;
        static QString                        mMonitorProfilePath;

        QSharedPointer<char>                  mHTransform;
        QString                               mProfileName;
};
}
#endif // PHOTOSTAGE_COLOR_H
