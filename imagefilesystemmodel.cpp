#include <QThread>
#include <QDebug>

#include "imagefilesystemmodel.h"
#include "imagefileloader.h"

ImageFileSystemModel::ImageFileSystemModel(QObject* parent) : QFileSystemModel(parent)
{
    mPixmapCache = new QHash<QModelIndex,QImage>();
    mThreadPool = new QThreadPool(this);
}

ImageFileSystemModel::~ImageFileSystemModel()
{
    mPixmapCache->clear();
    delete mPixmapCache;
}

QVariant ImageFileSystemModel::data(const QModelIndex &index, int role) const
{
    QVariant data = QFileSystemModel::data(index,role);
    if ( role == ImageFileSystemModel::FileImageRole)
    {
        // TODO: allocate on heap or stack?
        const QString path = data.toString();
        QImage image;
        if (mPixmapCache->contains(index))
        {
            // return cached image
            image = mPixmapCache->value(index);
        }
        else
        {
            // load image in background thread
            ImageFileLoader* loader = new ImageFileLoader(path,index);
            connect(loader,&ImageFileLoader::dataReady,this,&ImageFileSystemModel::imageLoaded);
            mThreadPool->start(loader);

            // Insert the dummy image here so that the we know the loader thread has been started
            mPixmapCache->insert(index,image);

            return QVariant();
        }

        return image;
    }
    else
    {
        return data;
    }

}

void ImageFileSystemModel::clearCache()
{
    mPixmapCache->clear();
}

void ImageFileSystemModel::imageLoaded(const QModelIndex &index, const QImage& pixmap)
{

    mPixmapCache->insert(index,pixmap);
    QVector<int> roles;
    roles.append(ImageFileSystemModel::FileImageRole);
    emit dataChanged(index,index,roles);
}

void ImageFileSystemModel::loaderError(QString error)
{
    qDebug() << "Can't load image: " <<error;
}
