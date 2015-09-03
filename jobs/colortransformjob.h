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

        ColorTransformJob(const Photo& photo);

        QVariant run();
        void finished(QVariant result);
        void error(const QString&);
        void cancel();

    signals:

        void imageReady(Photo photo, const QImage& image);

    private:

        Photo mPhoto;
};
}
#endif // PHOTOSTAGE_COLORTRANSFORMJOB_H
