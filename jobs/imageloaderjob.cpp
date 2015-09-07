#include <QDebug>
#include <QFileInfo>
#include <QThread>

#include <lcms2.h>

#include "constants.h"
#include "imageloaderjob.h"
#include "import/exivfacade.h"
#include "engine/dimensionoperation.h"
#include "io/jpegio.h"
#include "io/rawio.h"
#include "previewcache.h"

namespace PhotoStage
{
ImageLoaderJob::ImageLoaderJob(const Photo& photo, bool createPreview) :
    QObject(NULL),
    mPhoto(photo),
    mCreatePreview(createPreview),
    mExifUpdated(false)
{
    setName("ImageLoaderJob");
}

ImageLoaderJob::~ImageLoaderJob()
{
}

QVariant ImageLoaderJob::run()
{
    return startLoading(mPhoto);
}

void ImageLoaderJob::finished(QVariant result)
{
    if (mExifUpdated)
        emit exifUpdated(mPhoto);

    if (mCreatePreview)
    {
        QImage image = result.value<QImage>();
        emit   previewReady(mPhoto, image);
    }
    else
    {
        Image image = result.value<Image>();
        emit  imageReady(mPhoto, image);
    }
}

void ImageLoaderJob::error(const QString& error)
{
    qDebug() << "Error during image load" << error;
}

void ImageLoaderJob::cancel()
{
    mPhoto.setIsDownloading(false);
}

QVariant ImageLoaderJob::startLoading(Photo& photo)
{
    if (mCreatePreview)
    {
        QString key = QString::number(mPhoto.id());
        QImage  img = PreviewCache::globalCache()->get(key);

        if (!img.isNull())
            return QVariant::fromValue<QImage>(img);
    }

    Image image = loadImage(mPhoto.srcImagePath());

    if (mCreatePreview)
        return QVariant::fromValue<QImage>(preparePreview(image));
    else
        return QVariant::fromValue<Image>(image);
}

Image ImageLoaderJob::loadImage(const QString& path)
{
    // TODO: catch errors and emit error(QString)
    Image image;

    // load the file into a buffer first.
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug("Can't open file");
        return image;
    }
    QByteArray memFile = file.readAll();
    file.close();

    // rotate the image if necessary
    ExivFacade* ex = ExivFacade::createExivReader();

    if (!ex->openData(memFile))
    {
        qDebug() << "Error loading exif data from image";
        return image;
    }
    ExifInfo ex_info = ex->data();
    delete(ex);

    QString suffix = QFileInfo(path).suffix().toUpper();

    // TODO
    if (suffix == "NEF" || suffix == "CR2" || suffix == "CRW")
    {
        qDebug() << "Load raw" << path;

        RawIO rawIO;
        image = rawIO.fromFile(memFile, ex_info);
    }
    else
    {
        // Read using JPEG library
        //qDebug() << "Load jpg" << path;
        QByteArray iccProfile;

        image = JpegIO::fromFile(memFile, ex_info);
    }

    return image;
}

QImage ImageLoaderJob::preparePreview(const Image& image)
{
    mPhoto.exifInfo().width  = image.width();
    mPhoto.exifInfo().height = image.height();

    mExifUpdated = true;

    // Scale the image down to correct size.

    QImage scaled = image.toPreviewImage();

    // store the preview in the cache
    QString key = QString::number(mPhoto.id());

    PreviewCache::globalCache()->put(key, scaled);

    return scaled;
}
}
