#include <QCoreApplication>
#include <QDebug>
#include <QDir>

#include "colortransform.h"

#include "platform.h"
#include "utils.h"

namespace PhotoStage {
// init static variables
QHash<QString, ColorTransform> ColorTransform::mTransformCache;
QString                        ColorTransform::mMonitorProfilePath;

ColorTransform ColorTransform::getTransform(const QString& iden,
                                            const QString& from,
                                            const QString& to, Format inFormat,
                                            Format outFormat)
{
  if (mTransformCache.contains(iden))
  {
    return mTransformCache.value(iden);
  }
  else
  {
    ColorTransform transform(from, to, inFormat, outFormat);
    mTransformCache.insert(iden, transform);
    return transform;
  }
}

ColorTransform ColorTransform::getTransform(const QByteArray& srcProfile,
                                            const QString& to, Format inFormat,
                                            Format outFormat)
{
  ColorTransform transform(srcProfile, to, inFormat, outFormat);

  return transform;
}

QString ColorTransform::getMonitorProfilePath()
{
  // TODO: currently only return the profile for the primary monitor
  if (mMonitorProfilePath.isNull())
    mMonitorProfilePath = Platform::getMonitorProfilePath();

  return mMonitorProfilePath;
}

ColorTransform::ColorTransform() : mHTransform(NULL)
{
}

static void transform_delete(char* d)
{
  cmsHTRANSFORM transform = (cmsHTRANSFORM)d;

  cmsDeleteTransform(transform);
}

ColorTransform::ColorTransform(const QByteArray& srcProfile, const QString& to,
                               Format inFormat, Format outFormat)
{
  cmsHPROFILE hInProfile, hOutProfile;

  QString toProfile;

  if (to.at(0) == QDir::separator())
  {
    toProfile = to;
    qDebug() << "Loading monitor profile";
  }
  else {
#if defined(Q_OS_UNIX)
    toProfile = QCoreApplication::applicationDirPath() +
                "/../../PhotoStage/resources/Profiles/" + to + ".icc";
#else
    toProfile = QCoreApplication::applicationDirPath() +
                "/../Resources/Profiles/" + to + ".icc";
#endif
  }

  qDebug() << "Source profile supplied as data";
  qDebug() << "Loaded destination profile: " << toProfile;

  hInProfile =
      cmsOpenProfileFromMem(srcProfile.constData(), srcProfile.length());

  wchar_t str[1024] = {0};

  int res = cmsGetProfileInfo(hInProfile, cmsInfoDescription, "en", "US", str,
                              255 * sizeof(wchar_t));

  mProfileName = QString::fromWCharArray(str);

  hOutProfile = cmsOpenProfileFromFile(toProfile.toLocal8Bit().data(), "r");

#define MY_TYPE                                                                \
  (FLOAT_SH(1) | COLORSPACE_SH(PT_RGB) | CHANNELS_SH(3) | BYTES_SH(4) |        \
   DOSWAP_SH(1))

  cmsUInt32Number inputFormat  = MY_TYPE;
  cmsUInt32Number outputFormat = MY_TYPE;

  if (inFormat == FORMAT_RGB32)
    inputFormat = TYPE_BGRA_8;
  else if (inFormat == FORMAT_GRAYSCALE8)
    inputFormat = (COLORSPACE_SH(PT_RGB) | CHANNELS_SH(1) | BYTES_SH(1));
  else if (inFormat == FORMAT_BGR48_PLANAR)
    inputFormat = TYPE_BGR_16_PLANAR;
  else if (inFormat == FORMAT_RGB48_PLANAR)
    inputFormat = TYPE_RGB_16_PLANAR;

  if (outFormat == FORMAT_RGB32)
    outputFormat = TYPE_BGRA_8;
  else if (outFormat == FORMAT_BGR48_PLANAR)
    outputFormat = TYPE_BGR_16_PLANAR;
  else if (outFormat == FORMAT_RGB48_PLANAR)
    outputFormat = TYPE_RGB_16_PLANAR;

  mHTransform = QSharedPointer<char>(
      (char*)cmsCreateTransform(
          hInProfile, inputFormat, hOutProfile, outputFormat, INTENT_PERCEPTUAL,
          cmsFLAGS_CLUT_POST_LINEARIZATION | cmsFLAGS_CLUT_PRE_LINEARIZATION),
      transform_delete);

  cmsCloseProfile(hInProfile);
  cmsCloseProfile(hOutProfile);
}

ColorTransform::ColorTransform(const QString& from, const QString& to,
                               Format inFormat, Format outFormat)
{
  cmsHPROFILE hInProfile, hOutProfile;

#if defined(Q_OS_LINUX)
  QString fromProfile = QCoreApplication::applicationDirPath() +
                        "/../../PhotoStage/resources/Profiles/" + from + ".icc";
#else
  QString fromProfile = QCoreApplication::applicationDirPath() +
                        "/../Resources/Profiles/" + from + ".icc";
#endif
  mProfileName = from;

  qDebug() << "Loaded source profile: " << fromProfile;

  QString toProfile;

  if (to.at(0) == QDir::separator())
  {
    toProfile = to;
    qDebug() << "Loading monitor profile";
  }
  else {
#if defined(Q_OS_LINUX)
  QString toProfile = QCoreApplication::applicationDirPath() +
                        "/../../PhotoStage/resources/Profiles/" + to + ".icc";
#else
  QString toProfile = QCoreApplication::applicationDirPath() +
                        "/../Resources/Profiles/" + to + ".icc";
#endif
}
  qDebug() << "Loaded destination profile: " << toProfile;

  hInProfile  = cmsOpenProfileFromFile(fromProfile.toLocal8Bit().data(), "r");
  hOutProfile = cmsOpenProfileFromFile(toProfile.toLocal8Bit().data(), "r");

#define MY_TYPE                                                                \
  (FLOAT_SH(1) | COLORSPACE_SH(PT_RGB) | CHANNELS_SH(3) | BYTES_SH(4) |        \
   DOSWAP_SH(1))

  cmsUInt32Number inputFormat  = MY_TYPE;
  cmsUInt32Number outputFormat = MY_TYPE;

  if (inFormat == FORMAT_RGB32)
    inputFormat = TYPE_BGRA_8;
  else if (inFormat == FORMAT_GRAYSCALE8)
    inputFormat = (COLORSPACE_SH(PT_RGB) | CHANNELS_SH(1) | BYTES_SH(1));
  else if (inFormat == FORMAT_BGR48_PLANAR)
    inputFormat = TYPE_BGR_16_PLANAR;
  else if (inFormat == FORMAT_RGB48_PLANAR)
    inputFormat = TYPE_RGB_16_PLANAR;

  if (outFormat == FORMAT_RGB32)
    outputFormat = TYPE_BGRA_8;
  else if (outFormat == FORMAT_BGR48_PLANAR)
    outputFormat = TYPE_BGR_16_PLANAR;
  else if (outFormat == FORMAT_RGB48_PLANAR)
    outputFormat = TYPE_RGB_16_PLANAR;

  mHTransform = QSharedPointer<char>(
      (char*)cmsCreateTransform(
          hInProfile, inputFormat, hOutProfile, outputFormat, INTENT_PERCEPTUAL,
          cmsFLAGS_CLUT_POST_LINEARIZATION | cmsFLAGS_CLUT_PRE_LINEARIZATION),
      transform_delete);

  cmsCloseProfile(hInProfile);
  cmsCloseProfile(hOutProfile);
}

ColorTransform::~ColorTransform()
{
}

bool ColorTransform::isValid() const
{
  return mHTransform != NULL;
}

const QString& ColorTransform::profileName()
{
  return mProfileName;
}

Image ColorTransform::transformImage(const Image& inImage) const
{
  Image           outImage(inImage.size());
  const uint16_t* inbuf  = inImage.data();
  uint16_t*       outbuf = outImage.data();

  cmsDoTransform((cmsHTRANSFORM)mHTransform.data(), inbuf, outbuf,
                 inImage.width() * inImage.height());

  return outImage;
}

QImage ColorTransform::transformToQImage(const Image& inImage) const
{
  int      width  = inImage.width();
  int      height = inImage.height();
  uint8_t* buffer = new uint8_t[width * height * 4];

  cmsDoTransform((cmsHTRANSFORM)mHTransform.data(), (uint8_t*)inImage.data(),
                 buffer, width * height);

  for (int y = 0; y < height; y++)
  {
    uint8_t* outLine = &buffer[y * width * 4];

    for (int x = 0; x < width; x++)
      // set alpha value to 0xff (because little CMS does not)
      outLine[x * 4 + 3] = 0xff;
  }

  return QImage(buffer, width, height, 4 * width, QImage::Format_RGB32,
                (QImageCleanupFunction)deleteArray<uint8_t>, buffer);
}

QImage ColorTransform::transformQImage(const QImage& inImage) const
{
  int width  = inImage.width();
  int height = inImage.height();

  uint8_t* buffer = new uint8_t[width * height * 4];

  for (int y = 0; y < height; y++)
  {
    const uint8_t* inLine  = inImage.constScanLine(y);
    uint8_t*       outLine = &buffer[y * width * 4];

    cmsDoTransform((cmsHTRANSFORM)mHTransform.data(), inLine, outLine, width);

    // TODO: optimization.. littleCMS can convert in place
    // when source and dest image format organization are the same
    for (int x = 0; x < width; x++)
      // set alpha value to 0xff (because little CMS does not)
      outLine[x * 4 + 3] = 0xff;
  }

  return QImage(buffer, width, height, 4 * width, QImage::Format_RGB32,
                (QImageCleanupFunction)deleteArray<uint8_t>, buffer);
}

Image ColorTransform::transformFromQImage(const QImage& inImage) const
{
  Image outImage(inImage.size());
  int   height = inImage.height();
  int   width  = inImage.width();

  for (int y = 0; y < height; y++)
  {
    const uint8_t* inLine  = inImage.constScanLine(y);
    uint16_t*      outLine = outImage.scanLine(y);
    cmsDoTransform((cmsHTRANSFORM)mHTransform.data(), inLine, outLine, width);
  }
  return outImage;
}
} // namespace PhotoStage
