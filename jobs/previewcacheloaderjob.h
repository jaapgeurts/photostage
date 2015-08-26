#ifndef PHOTOSTAGE_PREVIEWCACHELOADERJOB_H
#define PHOTOSTAGE_PREVIEWCACHELOADERJOB_H

#include <QObject>
#include "runnable.h"
#include "photo.h"

namespace PhotoStage
{
class PreviewCacheLoaderJob :   public QObject, public Runnable
{
    Q_OBJECT

    public:

        PreviewCacheLoaderJob(const Photo& photo);

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
#endif // PHOTOSTAGE_PREVIEWCACHELOADERJOB_H
