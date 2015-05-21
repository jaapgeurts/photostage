#include <QThread>
#include <QDebug>

#include "imagefilesystemmodel.h"
#include "previewfileloader.h"
#include "widgets/tileview.h"

ImageFileSystemModel::ImageFileSystemModel(QObject* parent) : QFileSystemModel(parent)
{
    mPreviewInfoCache = new QHash<QModelIndex,PreviewInfo>();
    mThreadPool = new QThreadPool(this);
}

ImageFileSystemModel::~ImageFileSystemModel()
{
    mPreviewInfoCache->clear();
    delete mPreviewInfoCache;
}

QVariant ImageFileSystemModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid())
        return QVariant();
    QVariant data = QFileSystemModel::data(index,QFileSystemModel::FilePathRole);

    if ( role == TileView::PhotoRole)
    {
        // TODO: allocate on heap or stack?
        const QString path = data.toString();

        PreviewInfo info;

        if (mPreviewInfoCache->contains(index))
        {
            // return cached image
            info = mPreviewInfoCache->value(index);
        }
        else
        {

            info.filePath = path;
            // load image in background thread
            PreviewFileLoader* loader = new PreviewFileLoader(path,index);
            connect(loader,&PreviewFileLoader::dataReady,this,&ImageFileSystemModel::imageLoaded);
            mThreadPool->start(loader);

            // Insert the dummy image here so that the we know the loader thread has been started
            mPreviewInfoCache->insert(index,info);

            return QVariant();
        }
        QVariant v;
        v.setValue(info);
        return v;
    }
    else
    {
        return data;
    }

}

void ImageFileSystemModel::clearCache()
{
    mPreviewInfoCache->clear();
}

void ImageFileSystemModel::imageLoaded(const QModelIndex &index, const QImage& pixmap)
{

    PreviewInfo info = mPreviewInfoCache->value(index);
    info.image = pixmap;
    mPreviewInfoCache->insert(index,info);
    QVector<int> roles;
    roles.append(TileView::PhotoRole);
    emit dataChanged(index,index,roles);
}

void ImageFileSystemModel::loaderError(QString error)
{
    qDebug() << "Can't load image: " <<error;
}
