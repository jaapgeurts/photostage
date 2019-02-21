#ifndef IMAGEFILESYSTEMMODEL_H
#define IMAGEFILESYSTEMMODEL_H

#include <QAbstractListModel>
#include <QFileInfo>
#include <QHash>
#include <QImage>
#include <QModelIndex>
#include <QStringList>
#include <QThreadPool>

#include "previewinfo.h"

namespace PhotoStage {
class ImageFileSystemModel : public QAbstractListModel
{
  Q_OBJECT

public:
  ImageFileSystemModel(QObject* parent = 0);
  ~ImageFileSystemModel();

  QVariant data(const QModelIndex& index, int role) const;

  void clearCache();

  int         rowCount(const QModelIndex& parent = QModelIndex()) const;
  QModelIndex setRootPath(const QString& rootPath);

  QFileInfo fileInfo(const QModelIndex& index);

  void setIncludeSubdirs(bool include);
  bool includeSubdirs() const;

public slots:

  void imageLoaded(const QModelIndex& index, const QImage& pixmap);
  void loaderError(QString error);

private:
  QHash<QModelIndex, PreviewInfo>* mPreviewInfoCache;
  QThreadPool*                     mThreadPool;
  QString                          mRootPath;
  QStringList                      mFileList;
  QStringList                      mFilters;
  bool                             mIncludeSubdirs;

  void populate();
};
} // namespace PhotoStage
#endif // IMAGEFILESYSTEMMODEL_H
