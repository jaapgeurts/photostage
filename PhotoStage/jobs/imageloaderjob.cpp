#include <QDebug>
#include <QFileInfo>
#include <QThread>

#include "constants.h"
#include "database/databaseaccess.h"
#include "engine/colortransform.h"
#include "engine/dimensionoperation.h"
#include "imageloaderjob.h"
#include "import/exivfacade.h"
#include "io/jpegio.h"
#include "io/rawio.h"
#include "previewcache.h"

namespace PhotoStage {
ImageLoaderJob::ImageLoaderJob(const Photo& photo, bool createPreview)
    : QObject(NULL), mPhoto(photo), mCreatePreview(createPreview),
      mExifUpdated(false), mMustSaveParams(false)
{
  setName("ImageLoaderJob");
}

ImageLoaderJob::~ImageLoaderJob()
{
}

QVariant ImageLoaderJob::run()
{
  return startLoading();
}

void ImageLoaderJob::finished(QVariant result)
{
  if (mExifUpdated)
    emit exifUpdated(mPhoto);

  if (mMustSaveParams)
    emit saveParams(mPhoto, mRawParams);

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
  mPhoto.setIsDownloadingPreview(false);
  // TODO: check if we can cancel early
}

QVariant ImageLoaderJob::startLoading()
{
  if (mCreatePreview)
  {
    QString key = QString::number(mPhoto.id());
    QImage  img = PreviewCache::globalCache()->get(key);

    if (!img.isNull())
      return QVariant::fromValue<QImage>(img);
  }

  Image image = loadImage(mPhoto.srcImagePath());

  // TODO: check if updating exif is needed
  mPhoto.exifInfo().width  = image.width();
  mPhoto.exifInfo().height = image.height();

  mExifUpdated = true;

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
  delete (ex);

  QString suffix = QFileInfo(path).suffix().toUpper();

  ColorTransform toWorking;
  QString        dstColorSpace;

  if (mCreatePreview)
    dstColorSpace = PREVIEW_COLOR_SPACE;
  else
    dstColorSpace = WORKING_COLOR_SPACE;

  // TODO
  if (suffix == "NEF" || suffix == "CR2" || suffix == "CRW")
  {
    qDebug() << "Load raw" << path;

    // TODO: This field should be loaded from DB.
    mPhoto.setPhotoType(Photo::ContainerRAW);
    // passs the develop settings here to the rawIO
    // TODO: move db access back to the main thread.
    mRawParams =
        DatabaseAccess::developSettingDao()->getRawSettings(mPhoto.id());

    if (mRawParams.isNull())
    {
      mRawParams = QSharedPointer<DevelopRawParameters>(
          new DevelopRawParameters(ex_info));
      // TODO: get this value from settings
      mRawParams->algorithm = DevelopRawParameters::Bilinear;
      mMustSaveParams       = true;
    }

    RawIO rawIO(memFile, mRawParams, *ex_info.model);
    image = rawIO.image();
    // the raw file is always presented in XYZ
    // tdo assign profiles to the raw file.
    QString srcProfile = "D65_XYZ";
    toWorking          = ColorTransform::getTransform(
        srcProfile + dstColorSpace, srcProfile, dstColorSpace,
        ColorTransform::FORMAT_RGB48_PLANAR,
        ColorTransform::FORMAT_BGR48_PLANAR);
    mPhoto.exifInfo().profileName = "Camera Default";
  }
  else
  {
    // Read using JPEG library
    // qDebug() << "Load jpg" << path;
    mPhoto.setPhotoType(Photo::ContainerJPG);

    JpegIO jpegIO(memFile, ex_info);
    image                 = jpegIO.image();
    QByteArray iccProfile = jpegIO.colorProfile();

    // jpeg files are presented as is.
    // if a profile is present use that, otherwise assume sRGB
    if (!iccProfile.isEmpty())
    {
      // use the embedded JPeg profile

      toWorking = ColorTransform::getTransform(
          iccProfile, dstColorSpace, ColorTransform::FORMAT_BGR48_PLANAR,
          ColorTransform::FORMAT_BGR48_PLANAR);
      mPhoto.exifInfo().profileName = toWorking.profileName();
    }
    else
    {
      QString srcProfile = "sRGB";
      // Assume default JPEG images are in sRGB format.
      toWorking = ColorTransform::getTransform(
          srcProfile + dstColorSpace, srcProfile, dstColorSpace,
          ColorTransform::FORMAT_BGR48_PLANAR,
          ColorTransform::FORMAT_BGR48_PLANAR);
      mPhoto.exifInfo().profileName = "sRGB (Assumed)";
    }
  }

  return toWorking.transformImage(image);
}

QImage ImageLoaderJob::preparePreview(const Image& image)
{
  // Scale the image down to correct size.
  QImage scaled = image.toPreviewImage();

  // store the preview in the cache
  QString key = QString::number(mPhoto.id());

  PreviewCache::globalCache()->put(key, scaled);

  return scaled;
}
} // namespace PhotoStage
