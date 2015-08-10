#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QDir>

#include "photodao.h"
#include "databaseaccess.h"
#include "dbutils.h"
#include "previewcache.h"
#include "utils.h"

namespace PhotoStage
{
PhotoDAO::PhotoDAO(QObject* parent) :
    QObject(parent)
{
}

void PhotoDAO::setRating(const QList<Photo>& list, int rating)
{
    if (list.isEmpty())
        return;

    QSqlQuery q;
    QString   query = "update photo set rating=:rating where id in (:id)";

    QString   ids = joinIds(list);

    query.replace(":id", ids);
    q.prepare(query);
    q.bindValue(":rating", rating);
    q.exec();
    emit photosChanged(list);
}

void PhotoDAO::setFlag(const QList<Photo>& list, Photo::Flag flag)
{
    if (list.isEmpty())
        return;

    QSqlQuery q;
    QString   query = ("update photo set flag=:flag where id in (:id)");

    QString   ids = joinIds(list);
    query.replace(":id", ids);
    q.prepare(query);
    q.bindValue(":flag", (int)flag);
    q.exec();
    emit photosChanged(list);
}

void PhotoDAO::setColorLabel(const QList<Photo>& list, Photo::ColorLabel color)
{
    if (list.isEmpty())
        return;

    QSqlQuery q;
    QString   query = ("update photo set color=:color where id in (:id)");

    QString   ids = joinIds(list);

    query.replace(":id", ids);
    q.prepare(query);
    q.bindValue(":color", (int)color);
    q.exec();
    emit photosChanged(list);
}

void PhotoDAO::insertKeywords(const QStringList& words)
{
    if (words.isEmpty())
        return;

    // find keywords already in the database
    QSqlQuery   q;
    QStringList newWords = words;
    q.prepare("select id, keyword from keyword where keyword in (:list)");
    QString     list = words.join("','");
    q.bindValue(":list", list);
    q.exec();

    // remove those from the insert list
    while (q.next())
    {
        newWords.removeAll(q.value(1).toString());
    }

    // if there is nothing to insert then bail.
    if (newWords.isEmpty())
        return;

    // get the root id ( a single root is needed )
    q.clear();
    q.prepare("select id from keyword where parent_id is NULL");
    q.exec();
    long long parent;

    if (q.first())
        parent = q.value(0).toLongLong();
    else
    {
        // insert root because there is none.
        q.clear();
        q.exec("insert into keyword (keyword,parent_id) values('',NULL) ");
        parent = q.lastInsertId().toLongLong();
    }

    // insert all the keywords.
    DatabaseAccess::instance()->beginTransaction();
    q.clear();
    q.prepare("insert into keyword (keyword,parent_id) values ( :keyword , :parent )");
    q.bindValue(":parent", parent);
    QString word;
    foreach(word, newWords)
    {
        q.bindValue(":keyword", word);

        if (!q.exec())
            qDebug() << q.lastError();
    }
    // TODO: improve performance and don't rebuild tree on each insert.
    DatabaseAccess::instance()->endTransaction();
    rebuildKeywordTree(parent, 1);
    emit keywordsAdded();
}

void PhotoDAO::assignKeywords(const QStringList& words, const QList<Photo>& list)
{
    if (list.isEmpty())
        return;

    if (words.isEmpty())
        return;

    QSqlQuery q;
    QString   word;
    DatabaseAccess::instance()->beginTransaction();

    q.prepare(
        "insert into photo_keyword (photo_id, keyword_id) \
              select :photo_id, k.id \
              from keyword k \
              where k.keyword = :keyword");
    Photo info;
    foreach(info, list)
    {
        q.bindValue(":photo_id", info.id());
        foreach(word, words)
        {
            q.bindValue(":keyword", word);
            q.exec();
        }
    }
    DatabaseAccess::instance()->endTransaction();
    // TODO: consider photosChanged() vs keywordAssignmentsChanged()
    emit keywordsAssignmentChanged(list);
}

void PhotoDAO::unAssignKeywordsExcept(const QStringList& words, const QList<Photo>& list)
{
    if (list.isEmpty())
        return;

    if (words.isEmpty())
        return;

    QSqlQuery q;

    QString   query =
        "delete from photo_keyword \
            where photo_id in (:photo_id) \
            and keyword_id not in  \
            ( select id from keyword where keyword in (':keywords'))";

    QString photo_id = joinIds(list);

    QString keywords = words.join("','");

    query.replace(":photo_id", photo_id);
    query.replace(":keywords", keywords);

    if (!q.exec(query))
    {
        qDebug() << q.lastError();
        qDebug() << q.lastQuery();
    }

    emit photosChanged(list);
}

void PhotoDAO::beginImport()
{
    mLastPath.clear();
    mLastPathId = -1;
}

void PhotoDAO::importPhoto(long long collectionid, const QFileInfo& file, const ImportOptions& options)
{
    long long ret = -1;

    qDebug() << "Importing file" << file.canonicalFilePath();
    QString fileName = file.fileName();
    QString srcpath  = file.canonicalFilePath();
    QString dstdir   = options.destinationDir().canonicalFilePath();
    QString dstpath  = dstdir + QDir::separator() + fileName;

    bool    copySuccess = false;

    //take action on the file(in case of copy & move)
    switch (options.importMode())
    {
        case ImportOptions::ImportAdd:
            // do nothing. just import the filepaths into the DB
            dstpath     = srcpath;
            dstdir      = file.canonicalPath();
            copySuccess = true;
            break;

        case ImportOptions::ImportCopy:

            copySuccess = QFile::copy(srcpath, dstpath);

            if (!copySuccess)
                qDebug() << "File copy failed from" << srcpath << "to" <<
                    dstpath;
            break;

        case ImportOptions::ImportMove:
            // move each file
            copySuccess = QFile::rename(srcpath, dstpath);     // rename moves if on different filesystems

            if (!copySuccess)
                qDebug() << "File move failed from" << srcpath << "to" <<
                    dstpath;
            break;
    }

    if (!copySuccess)
        return;

    if (mLastPath != dstdir)
    {
        mLastPath = dstdir;
        // TODO: find way to prevent this call
        QStringList pathlist = dstdir.split(QDir::separator(), QString::KeepEmptyParts);
        mLastPathId = DatabaseAccess::pathDao()->createPaths(pathlist);
    }

    // read all exif data.
    ExivFacade* ex = ExivFacade::createExivReader();

    ExifInfo    ei;

    if (!ex->openFile(srcpath))
        qDebug() << "Skipping exif info";
    else
        ei = ex->data();
    delete(ex);

    QFileInfo fi(srcpath);
    long long hash = computeImageFileHash(fi);

    if (ei.dateTimeOriginal == nullptr)
        ei.dateTimeOriginal = fi.lastModified();

    if (ei.dateTimeDigitized == nullptr)
        ei.dateTimeDigitized = fi.created();

    QSqlQuery q;
    q.prepare(
        "insert into photo (path_id,filename,photo_hash, iso, exposure_time, \
        focal_length, datetime_original, datetime_digitized, rotation, \
        longitude, lattitude, copyright, artist, aperture, flash, lens_name,  \
        make, model, width, height ) \
        values (:path, :filename,:hash,:iso,:exposure_time,:focal_length,\
            :datetime_original,:datetime_digitized,:rotation,:longitude, \
            :lattitude,:copyright,:artist,:aperture,:flash,:lens_name, \
            :make, :model, :width, :height)");
    q.bindValue(":path", mLastPathId);
    q.bindValue(":filename", fileName);
    q.bindValue(":hash", hash);
    q.bindValue(":iso", setDbValue(ei.isoSpeed));
    q.bindValue(":exposure_time", setDbValue(ei.exposureTime));
    q.bindValue(":focal_length", setDbValue(ei.focalLength));
    q.bindValue(":datetime_original", setDbValue(ei.dateTimeOriginal));
    q.bindValue(":datetime_digitized", setDbValue(ei.dateTimeDigitized));
    q.bindValue(":rotation", ei.rotation);

    if (ei.location != nullptr)
    {
        q.bindValue(":longitude", ei.location->longitude());
        q.bindValue(":lattitude", ei.location->latitude());
    }
    else
    {
        q.bindValue(":longitude", QVariant(QVariant::Double));
        q.bindValue(":lattitude", QVariant(QVariant::Double));
    }
    q.bindValue(":copyright", setDbValue(ei.copyright));
    q.bindValue(":artist", setDbValue(ei.artist));
    q.bindValue(":aperture", setDbValue(ei.aperture));
    q.bindValue(":flash", setDbValue(ei.flash));
    q.bindValue(":lens_name", setDbValue(ei.lensName));
    q.bindValue(":make", setDbValue(ei.make));
    q.bindValue(":model", setDbValue(ei.model));
    q.bindValue(":width", ei.width);
    q.bindValue(":height", ei.height);

    if (!q.exec())
    {
        qDebug() << "Can't insert" << fileName << "into DB";
        qDebug() << q.lastError();
    }
    else
    {
        ret = q.lastInsertId().toLongLong();
        // make sure the global preview cache does not store an image with the same key
        // this is probably a left over cache entry from before.
        PreviewCache::globalCache()->remove(QString::number(ret));
    }

    if (ret != -1)
    {
        QList<long long> list;
        list << ret;
        DatabaseAccess::collectionDao()->addPhotosToCollection(collectionid, list);
        emit photosAdded(mLastPathId, list);
    }
}

// return keywords for the selected photos, and the count each keyword appears
QMap<QString, int> PhotoDAO::getPhotoKeywordsCount(const QList<Photo>& list) const
{
    QSqlQuery q;
    QString   query =
        "select k.keyword, count(pk.photo_id) \
            from keyword k, photo_keyword pk on k.id = pk.keyword_id \
            where pk.photo_id in (:photo_ids) \
            group by k.keyword order by k.keyword ";
    QString   photo_ids = joinIds(list);

    query.replace(":photo_ids", photo_ids);

    if (!q.exec(query))
    {
        qDebug() << q.lastError();
        qDebug() << q.lastQuery();
    }

    QMap<QString, int> dict;

    while (q.next())
    {
        dict.insert(q.value(0).toString(), q.value(1).toInt());
    }
    return dict;
}

QStringList PhotoDAO::getPhotoKeywords(const Photo& photo) const
{
    QSqlQuery q;

    q.prepare(
        "select k.keyword\
            from keyword k, photo_keyword pk on k.id = pk.keyword_id \
            where pk.photo_id = :photo_id ");
    q.bindValue(":photo_id", photo.id());

    if (!q.exec())
    {
        qDebug() << q.lastError();
        qDebug() << q.lastQuery();
    }
    QStringList list;

    while (q.next())
    {
        list << q.value(0).toString();
    }
    return list;
}

QList<Photo> PhotoDAO::getPhotosById(const QList<long long> idList) const
{
    QSqlQuery q;
    QString   query = QString(
        "select p.id, p.filename, d.path ,p.rating,p.color,p.flag, \
                p.iso, p.aperture,p.exposure_time, p.focal_length, p.datetime_original, \
                p.datetime_digitized, p.rotation, p.lattitude,p.longitude, \
                p.copyright, p.artist, p.flash, p.lens_name, p.make,  p.model, \
                p.width, p.height  \
              from photo p join (select group_concat(ancestor.directory ,:separator) as path, \
              child.* from path child join path ancestor \
              on child.lft >= ancestor.lft \
              and child.lft <= ancestor.rgt \
              group by child.lft \
              order by child.lft) as d on p.path_id = d.id \
              and p.id in (:photoids);");

    QString photoids = joinIds(idList);

    query.replace(":photoids", photoids);
    q.prepare(query);
    q.bindValue(":separator", QDir::separator());

    QList<Photo> list;

    if (!q.exec() && !q.isValid())
    {
        qDebug() << "Query failed" << q.executedQuery();
        qDebug() << q.lastError();
        return list;
    }

    while (q.next())
    {
        Photo p(q);
        p.setKeywords(getPhotoKeywords(p));
        list.append(p);
    }
    return list;
}

void PhotoDAO::deletePhotos(const QList<Photo>& list, bool deleteFile)
{
    if (list.isEmpty())
        return;

    QSqlQuery q;
    QString   photo_ids;

    // create a list of id's
    QList<QString> paths;

    foreach(Photo info, list)
    {
        photo_ids += QString::number(info.id()) + ",";
        paths << info.srcImagePath();
    }
    photo_ids.chop(1);

    // first delete any keyword references
    QString query = "delete from photo_keyword where photo_id in (:photo_ids)";
    query.replace(":photo_ids", photo_ids);

    if (!q.exec(query))
    {
        qDebug() << q.lastError();
        qDebug() << q.lastQuery();
        return;
    }

    // also remove any references in all collections.

    // now delete the photos from the main library.
    query = "delete from photo where id in (:photo_ids)";
    query.replace(":photo_ids", photo_ids);

    if (!q.exec(query))
    {
        qDebug() << q.lastError();
        qDebug() << q.lastQuery();
        return;
    }

    // Finally delete the actual file
    if (deleteFile)
    {
        foreach(QString path, paths)
        {
            QFile f(path);

            if (!f.remove())
                qDebug() << "File" << path << "could not be removed";
        }
    }
    emit photosDeleted(list);
}

void PhotoDAO::getLeftRightForCollectionId(long long collection_id, long long& lft, long long& rgt) const
{
    QSqlQuery q;

    q.prepare("select lft,rgt from collection where id = :id;");
    q.bindValue(":id", collection_id);

    if (!q.exec() && !q.isValid())
    {
        qDebug() << "Query failed" << q.executedQuery();
        qDebug() << q.lastError();
        return;
    }
    q.next();

    lft = q.value(0).toLongLong();
    rgt = q.value(1).toLongLong();
}

void PhotoDAO::getLeftRightForPathId(long long path_id, long long& lft, long long& rgt) const
{
    QSqlQuery q;

    q.prepare("select lft,rgt from path where id = :id;");
    q.bindValue(":id", path_id);

    if (!q.exec() && !q.isValid())
    {
        qDebug() << "Query failed" << q.executedQuery();
        qDebug() << q.lastError();
        return;
    }
    q.next();

    lft = q.value(0).toLongLong();
    rgt = q.value(1).toLongLong();
}

/**
 * @brief PhotoWorkUnit::getPhotosByPath Return a list of photo's that are in the path identified by path id.
 *      If includeSubDirs is true, then all photos under this path are returned as well.
 * @param path_id
 * @param includeSubDirs
 * @return
 */
QList<Photo> PhotoDAO::getPhotosByPath(long long path_id, bool includeSubDirs) const
{
    QSqlQuery    q;

    QList<Photo> list;
    q.clear();

    if (includeSubDirs)
    {
        long long lft, rgt;
        getLeftRightForPathId(path_id, lft, rgt);

        q.prepare(
            "select p.id, p.filename, d.path,p.rating,p.color,p.flag, \
              p.iso, p.aperture,p.exposure_time, p.focal_length, p.datetime_original, \
              p.datetime_digitized, p.rotation, p.lattitude,p.longitude, \
              p.copyright, p.artist, p.flash, p.lens_name, p.make,  p.model, \
                p.width, p.height  \
            from photo p join (select group_concat(ancestor.directory ,:separator) as path, \
            child.* from path child join path ancestor \
            on child.lft >= ancestor.lft \
            and child.lft <= ancestor.rgt \
            group by child.lft \
            order by child.lft) as d on p.path_id = d.id \
            and d.lft between :lft and :rgt;" );
        q.bindValue(":lft", lft);
        q.bindValue(":rgt", rgt);
    }
    else
    {
        q.prepare(
            "select p.id, p.filename, d.path,p.rating,p.color,p.flag, \
              p.iso, p.aperture,p.exposure_time, p.focal_length, p.datetime_original, \
              p.datetime_digitized, p.rotation, p.lattitude,p.longitude, \
              p.copyright, p.artist, p.flash, p.lens_name, p.make,  p.model, \
                p.width, p.height  \
            from photo p join (select group_concat(ancestor.directory ,:separator) as path, \
            child.* from path child join path ancestor \
            on child.lft >= ancestor.lft \
            and child.lft <= ancestor.rgt \
            group by child.lft \
            order by child.lft) as d on p.path_id = d.id \
            and p.path_id = :pathid;" );
        q.bindValue(":pathid", path_id);
    }
    q.bindValue(":separator", QDir::separator());

    if (!q.exec() && !q.isValid())
    {
        qDebug() << "Query failed" << q.executedQuery();
        qDebug() << q.lastError();
        return list;
    }

    while (q.next())
    {
        Photo p(q);
        p.setKeywords(getPhotoKeywords(p));
        list.append(p);
    }
    return list;
}

QList<Photo> PhotoDAO::getPhotosByCollectionId(long long collection_id, bool includeSubDirs) const
{
    QSqlQuery    q;

    QList<Photo> list;
    q.clear();

    if (includeSubDirs)
    {
        long long lft, rgt;
        getLeftRightForCollectionId(collection_id, lft, rgt);

        // FIXME: this is not correct.
        q.prepare(
            "select p.id, p.filename, d.path,p.rating,p.color,p.flag, \
              p.iso, p.aperture,p.exposure_time, p.focal_length, p.datetime_original, \
              p.datetime_digitized, p.rotation, p.lattitude,p.longitude, \
              p.copyright, p.artist, p.flash, p.lens_name, p.make,  p.model, \
                p.width, p.height  \
            from photo p join (select group_concat(ancestor.directory ,:separator) as path, \
            child.* from path child join path ancestor \
            on child.lft >= ancestor.lft \
            and child.lft <= ancestor.rgt \
            group by child.lft \
            order by child.lft) as d on p.path_id = d.id \
                join collection_photo cp on p.id =  cp.photo_id \
                join collection c on cp.collection_id = c.id \
            and c.lft between :lft and :rgt;" );
        q.bindValue(":lft", lft);
        q.bindValue(":rgt", rgt);
    }
    else
    {
        q.prepare(
            "select p.id, p.filename, d.path,p.rating,p.color,p.flag, \
              p.iso, p.aperture,p.exposure_time, p.focal_length, p.datetime_original, \
              p.datetime_digitized, p.rotation, p.lattitude,p.longitude, \
              p.copyright, p.artist, p.flash, p.lens_name, p.make,  p.model, \
                p.width, p.height  \
            from photo p join (select group_concat(ancestor.directory ,:separator) as path, \
            child.* from path child join path ancestor \
            on child.lft >= ancestor.lft \
            and child.lft <= ancestor.rgt \
            group by child.lft \
            order by child.lft) as d on p.path_id = d.id \
            and cp.collection_id = :collectionid \
            join collection_photo cp on p.id =  cp.photo_id;" );
        q.bindValue(":collectionid", collection_id);
    }
    q.bindValue(":separator", QDir::separator());

    if (!q.exec() && !q.isValid())
    {
        qDebug() << "Query failed" << q.executedQuery();
        qDebug() << q.lastError();
        return list;
    }

    while (q.next())
    {
        Photo p(q);
        p.setKeywords(getPhotoKeywords(p));
        list.append(p);
    }
    return list;
}

long long PhotoDAO::rebuildKeywordTree(long long parent_id, long long left)
{
    // the right value of this node is the left value + 1

    long long right = left + 1;

    // get all children of this node

    QSqlQuery q;

    q.prepare("select id from keyword where parent_id = :parent_id;");
    q.bindValue(":parent_id", parent_id);

    q.exec();

    while (q.next())
    {
        long long id = q.value(0).toLongLong();
        right = rebuildKeywordTree(id, right);
    }

    q.prepare("update keyword set lft=:left, rgt=:right where id=:parent_id;");

    q.bindValue(":left", left);
    q.bindValue(":right", right);
    q.bindValue(":parent_id", parent_id);

    q.exec();

    // return the right value of this node + 1

    return right + 1;
}

void PhotoDAO::updateExifInfo(Photo& photo)
{
    QSqlQuery       q;

    const ExifInfo& ei = photo.exifInfo();

    q.prepare(
        "update photo set width=:width, height=:height where id = :photoid");
    q.bindValue(":width", ei.width);
    q.bindValue(":height", ei.height);
    q.bindValue(":photoid", photo.id());

    if (!q.exec())
    {
        qDebug() << "Query failed" << q.executedQuery();
        qDebug() << q.lastError();
    }

    QList<Photo> list;
    list << photo;
    emit         photosChanged(list);
}

void PhotoDAO::regenerateHash(Photo& photo)
{
    QString   path = photo.srcImagePath();
    // only scan the first 1MB
    long long hash = computeImageFileHash(path);

    photo.setHash(hash);

    QSqlQuery q;
    q.prepare("update photo set photo_hash=:hash where id = :photoid");
    q.bindValue(":hash", photo.hash());
    q.bindValue(":photoid", photo.id());

    if (!q.exec())
    {
        qDebug() << "Query failed" << q.executedQuery();
        qDebug() << q.lastError();
    }

    QList<Photo> list;
    list << photo;
    emit         photosChanged(list);
}

bool PhotoDAO::IsInLibrary(long long hash) const
{
    QSqlQuery q;

    q.prepare("select count(*) from photo where photo_hash = :hash");
    q.bindValue(":hash", hash);

    if (!q.exec())
    {
        qDebug() << "Query failed" << q.executedQuery();
        qDebug() << q.lastError();
    }

    if (!q.next())
        qDebug() << "select count(*) for hash returned no results!??";
    int res = q.value(0).toInt();

    if (res > 1)
        qDebug() << "There are two photos with the same hash";
    return res == 1;
}
}
