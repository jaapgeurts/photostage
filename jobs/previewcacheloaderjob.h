#ifndef PREVIEWCACHELOADERJOB_H
#define PREVIEWCACHELOADERJOB_H

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
        void error(const QString& error);

    signals:

        void imageReady(Photo photo, const QImage& image);

    private:

        Photo mPhoto;
};
}
#endif // PREVIEWCACHELOADERJOB_H
