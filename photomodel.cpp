#include <QImage>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

#include "constants.h"
#include "photomodel.h"
#include "imagefileloader.h"
#include "widgets/tileview.h"

PhotoModel::PhotoModel(QObject* parent) :
    QAbstractListModel(parent),
    mPreviewCache(QStandardPaths::writableLocation(QStandardPaths::CacheLocation),parent)
{
    //
    // create table photo (
    //    id integer primary key AUTOINCREMENT,
    //    path integer,
    //    filename varchar,
    //            iso integer,
    //            shutter_speed float,
    //            float focal_length,
    //            datetime_taken text,
    //            hash varchar,
    //            rating integer,
    //            color integer,
    //            flag integer)")))

    mWorkUnit   = PhotoWorkUnit::instance();
    mThreadPool = new QThreadPool(this);
}

PhotoModel::~PhotoModel()
{
    qDeleteAll(mPhotoInfoList);
    mPhotoInfoList.clear();
}

QVariant PhotoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() > mPhotoInfoList.size())
    {
        qDebug() << "SqlPhotoModel::data() Invalid row requested";
        return QVariant();
    }

    Photo* info;

    if (role == TileView::PhotoRole)
    {
        info = mPhotoInfoList.at(index.row());

        if (info->libraryPreview().isNull())
        {
            QString key = QString::number(info->id);
            QImage  img = mPreviewCache.get(key);

            if (img.isNull() && !mPhotoInfoMap.contains(index))
            {
                // If not available return
                // load image in background thread
                ImageFileLoader* loader = new ImageFileLoader(index, info->srcImagePath());
                connect(loader,&ImageFileLoader::dataReady,this,&PhotoModel::imageLoaded);
                mThreadPool->start(loader);
                mPhotoInfoMap.insert(index,info);
            }
            info->setLibraryPreview(img);
            info->setOriginal(img);
        }

        return QVariant::fromValue<Photo*>(info);
    }
    else if (role == Qt::DisplayRole)
    {
        return QString(mPhotoInfoList.at(index.row())->srcImagePath());
    }
    else
        return QVariant();
}

void PhotoModel::imageLoaded(const QVariant& ref, const QImage& image)
{
    QModelIndex index   = ref.value<QModelIndex>();
    QImage      preview = image.scaled(QSize(PREVIEW_IMG_WIDTH,PREVIEW_IMG_HEIGHT),Qt::KeepAspectRatio,Qt::SmoothTransformation);

    Photo*      info = mPhotoInfoMap.value(index);

    QString key = QString::number(info->id);
    mPreviewCache.put(key,preview);

    info->setLibraryPreview(preview);
    mPhotoInfoMap.remove(index);

    QVector<int> roles;
    emit dataChanged(index,index,roles);
}

void PhotoModel::onReloadPhotos(PhotoModel::SourceType source, long long pathId)
{
    beginResetModel();

    if (source == SourceFiles)
    {
        mPhotoInfoList = mWorkUnit->getPhotosByPath(pathId);
    }
    else if (source == SourceCollection)
    {
        // TODO: not yet implemented
        //mPhotoInfoList = mWorkUnit->getPhotosByCollection(id);
    }
    // should also cancel all open threads
    endResetModel();
}

bool PComp(const Photo* const & a, const Photo* const & b)
{
    return a->id < b->id;
}

void PhotoModel::refreshData(const QList<Photo*> &list )
{
    // for now just emit that all data has changed. the tileview doesnt check anyway
    emit dataChanged(index(0,0),index(rowCount(QModelIndex()) - 1,0));
}

void PhotoModel::addData(const QList<long long>& idList)
{
    // figure out what was changed, what is new and what has been added
    int           start = rowCount(QModelIndex());

    QList<Photo*> list = mWorkUnit->getPhotosById(idList);
    beginInsertRows(QModelIndex(),start,list.size() - 1);
    Photo*        info;
    foreach(info, list)
    {
        mPhotoInfoList.append(info);
    }
    //    insertRows(start,list.size());
    endInsertRows();
}

QVariant PhotoModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int /*role*/) const
{
    return QVariant();
}

int PhotoModel::rowCount(const QModelIndex & /*parent*/) const
{
    return mPhotoInfoList.size();
}
