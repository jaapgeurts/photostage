#include <QDebug>

#include "constants.h"
#include "exivfacade.h"
#include "previewfileloader.h"

namespace PhotoStage {
PreviewFileLoader::PreviewFileLoader(const QString&     path,
                                     const QModelIndex& index)
{
  mModelIndex = index;
  mPath       = path;
  // qDebug() << "worker thread " << mModelIndex.row() <<" created";
}

PreviewFileLoader::~PreviewFileLoader()
{
  // qDebug() << "worker thread " << mModelIndex.row() <<" deleted";
}

void PreviewFileLoader::run()
{
  QImage pixmap;

  // make sure this function doesn't crash because of some exiv issues.
  ExivFacade* exiv = ExivFacade::createExivReader();

  if (!exiv->openFile(mPath))
  {
    qDebug() << "Failed to load embedded preview";
    return;
  }
  qDebug() << "Reading image:" << mPath;

  pixmap = exiv->getPreview();

  // if the image contained no thumbnail attempt to load it from disk directly
  if (pixmap.isNull())
    pixmap.load(mPath);
  QImage image = pixmap.scaled(QSize(PREVIEW_IMG_WIDTH, PREVIEW_IMG_HEIGHT),
                               Qt::KeepAspectRatio);

  emit dataReady(mModelIndex, image);
  delete exiv;
}
} // namespace PhotoStage
