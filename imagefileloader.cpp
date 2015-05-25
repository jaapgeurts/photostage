#include <QDebug>
#include <QThread>
#include "constants.h"
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
    // TODO: catch errors and emit error(QString)
    QImage pixmap = QImage(mPath);
    QImage image;
    if (pixmap.isNull())
        qDebug() << "ImageFileLoader::run() Can't load image" << mPath;
    else
      image = pixmap.scaled(QSize(PREVIEW_IMG_WIDTH,PREVIEW_IMG_HEIGHT),Qt::KeepAspectRatio);
    emit dataReady(mModelIndex,image);
}


