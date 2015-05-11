#include <QDebug>
#include <QThread>

#include "imagefileloader.h"

ImageFileLoader::ImageFileLoader(const QString& path,const QModelIndex& index)
{
    mModelIndex = index;
    mPath = path;
    //qDebug() << "worker thread " << mModelIndex.row() <<" created";
}

ImageFileLoader::~ImageFileLoader()
{
  //qDebug() << "worker thread " << mModelIndex.row() <<" deleted";
}

void ImageFileLoader::run()
{
   // qDebug() << "worker thread " << mModelIndex.row() <<" started";

    // TODO: catch errors and emit error(QString)
    QImage pixmap = QImage(mPath);
    QImage image = pixmap.scaled(QSize(256,256),Qt::KeepAspectRatio);
    emit dataReady(mModelIndex,image);
}


