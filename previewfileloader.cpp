#include <QDebug>

#include "exivfacade.h"
#include "previewfileloader.h"
#include "constants.h"


PreviewFileLoader::PreviewFileLoader(const QString& path,const QModelIndex& index)
{
    mModelIndex = index;
    mPath = path;
    //qDebug() << "worker thread " << mModelIndex.row() <<" created";
}

PreviewFileLoader::~PreviewFileLoader()
{
  //qDebug() << "worker thread " << mModelIndex.row() <<" deleted";
}

void PreviewFileLoader::run()
{
    ExivFacade *exiv = ExivFacade::createExivReader();
    qDebug() << "Reading image:" << mPath;
    QImage pixmap = exiv->thumbnail(mPath);
    QImage image = pixmap.scaled(QSize(PREVIEW_IMG_WIDTH,PREVIEW_IMG_HEIGHT),Qt::KeepAspectRatio);
    emit dataReady(mModelIndex,image);
    delete exiv;
}


