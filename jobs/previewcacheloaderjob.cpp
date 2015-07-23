#include <QDebug>

#include "previewcacheloaderjob.h"
#include "previewcache.h"

namespace PhotoStage
{
PreviewCacheLoaderJob::PreviewCacheLoaderJob(const Photo& photo) :
    mPhoto(photo)
{
    setName("PreviewCacheLoaderJob");
}

QVariant PreviewCacheLoaderJob::run()
{

    QString key = QString::number(mPhoto.id());
    QImage  img = PreviewCache::globalCache()->get(key);

    return img;
}

void PreviewCacheLoaderJob::finished(QVariant result)
{
    emit imageReady(mPhoto, result.value<QImage>());
}

void PreviewCacheLoaderJob::error(const QString& error)
{
}

void PreviewCacheLoaderJob::cancel()
{
    mPhoto.setIsDownloading(false);
}

}
