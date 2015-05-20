#include <QImage>
#include <QDir>

#include "sqlphotomodel.h"
#include "imagefileloader.h"

SqlPhotoModel::SqlPhotoModel(QObject *parent): QAbstractListModel(parent)
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

    mMainQuery = new QSqlQuery();
    QString query = QString("with recursive cte as ( \
                            select id,directory \
                            from path \
                            where parent_id is NULL \
                            union all \
                            select t.id as id , cte.directory || :separator || t.directory as directory \
                            from cte, path t \
                            where cte.id = t.parent_id \
            ) select p.id, p.filename, c.directory,p.rating,p.color,p.flag \
            from cte c, photo p \
            where c.id = p.path_id \
            order by p.id");

            mMainQuery->prepare(query);
    mMainQuery->bindValue(":separator",QDir::separator());

    if (!mMainQuery->exec())
        qDebug() << mMainQuery->lastError();

    if (!mMainQuery->first())
        qDebug() << "No records";

    //   mPathDict = new QHash<int, QString>();

    mPhotoInfoCache = new QHash<QModelIndex,SqlPhotoInfo>();
    mThreadPool = new QThreadPool(this);

}

SqlPhotoModel::~SqlPhotoModel()
{
    mPhotoInfoCache->clear();
    delete mPhotoInfoCache;
    delete mMainQuery;
}

//Qt::ItemFlags SqlPhotoModel::flags(const QModelIndex &index) const
//{
//  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
//}

QVariant SqlPhotoModel::data(const QModelIndex &index, int /*role*/) const
{

    if (!index.isValid())
        return QVariant();

    SqlPhotoInfo info;

    if (!mMainQuery->seek(index.row()))
    {
        qDebug() << "SqlPhotoModel::data() Invalid row requested";
        return QVariant();
    }

    if (mPhotoInfoCache->contains(index))
    {
        // return cached image
        info = mPhotoInfoCache->value(index);
    }
    else
    {
        info.id = mMainQuery->value(0).toInt();
        QString filename = mMainQuery->value(1).toString();
        QString path = mMainQuery->value(2).toString();
        info.fileName = path + QDir::separator() + filename;
        if (mMainQuery->value(3).isNull())
            info.setRating(0);
        else
          info.setRating(mMainQuery->value(3).toInt());
        info.setColorLabel((SqlPhotoInfo::ColorLabel)mMainQuery->value(4).toInt());
        info.setFlag((SqlPhotoInfo::Flag)mMainQuery->value(5).toInt());

        // load image in background thread
        ImageFileLoader* loader = new ImageFileLoader(info.fileName,index);
        connect(loader,&ImageFileLoader::dataReady,this,&SqlPhotoModel::imageLoaded);
        mThreadPool->start(loader);

        // Insert the dummy image here so that the we know the loader thread has been started
        mPhotoInfoCache->insert(index,info);

        return QVariant();
    }

    QVariant v;
    v.setValue(info);
    return v;

}

void SqlPhotoModel::imageLoaded(const QModelIndex &index, const QImage& pixmap)
{

    SqlPhotoInfo info = mPhotoInfoCache->value(index);
    info.image = pixmap;
    mPhotoInfoCache->insert(index,info);
    QVector<int> roles;
    //roles.append(ImageFileSystemModel::FileImageRole);
    emit dataChanged(index,index,roles);
}

void SqlPhotoModel::updateData(const QList<QModelIndex>& list )
{
    mMainQuery->exec();

    // remove values from the cache
    QModelIndex start,end;
    QModelIndex index;
    start = index;
    end = index;
    foreach (index, list) {
        if (index.row()<start.row())
            start = index;
        if (index.row()>end.row())
            end = index;
        mPhotoInfoCache->remove(index);
    }
    QVector<int> roles;
    emit dataChanged(start,end,roles);

}



QVariant SqlPhotoModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int /*role*/) const
{
return QVariant();
}

int SqlPhotoModel::rowCount(const QModelIndex &/*parent*/) const
{
    mMainQuery->last();
    int count = mMainQuery->at()+1;

    return count;
}
