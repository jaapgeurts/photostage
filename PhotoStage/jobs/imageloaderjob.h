#ifndef PHOTOSTAGE_PREVIEWGENERATORJOB_H
#define PHOTOSTAGE_PREVIEWGENERATORJOB_H

#include <QModelIndex>
#include <QObject>
#include <QString>

#include "photo.h"
#include "runnable.h"

namespace PhotoStage {
class ImageLoaderJob : public QObject, public Runnable
{
  Q_OBJECT

public:
  explicit ImageLoaderJob(const Photo& photo, bool createPreview = true);
  ~ImageLoaderJob();

public:
  // Runnable interface
  QVariant run();
  void     finished(QVariant result);
  void     error(const QString& error);
  void     cancel();

signals:

  void previewReady(Photo photo, const QImage& image);
  void imageReady(Photo photo, const Image& image);
  void exifUpdated(Photo photo);
  void saveParams(Photo                                       photo,
                  const QSharedPointer<DevelopRawParameters>& params);

private:
  Photo                                mPhoto;
  bool                                 mCreatePreview;
  bool                                 mExifUpdated;
  QSharedPointer<DevelopRawParameters> mRawParams;
  bool                                 mMustSaveParams;

  QVariant startLoading();
  QImage   preparePreview(const Image& image);
  Image    loadImage(const QString& path);
};
} // namespace PhotoStage
#endif // PHOTOSTAGE_PREVIEWGENERATORJOB_H
