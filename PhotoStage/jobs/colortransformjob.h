#ifndef PHOTOSTAGE_COLORTRANSFORMJOB_H
#define PHOTOSTAGE_COLORTRANSFORMJOB_H

#include <QObject>
#include <QImage>

#include "runnable.h"
#include "photo.h"

namespace PhotoStage
{
class ColorTransformJob  : public QObject, public Runnable
{
    Q_OBJECT

    public:

        enum ConversionType
        {
            Preview = 1,
            Develop = 2,
        };

        ColorTransformJob(const Photo& photo, ConversionType type);

        QVariant run();
        void finished(QVariant result);
        void error(const QString&);
        void cancel();

    signals:

        void imageReady(Photo photo, const QImage& image);

    private:

        Photo          mPhoto;
        ConversionType mType;
};
}
#endif // PHOTOSTAGE_COLORTRANSFORMJOB_H
