#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QDir>

#include "photodao.h"
#include "utils.h"

namespace PhotoStage
{
void PhotoDAO::beginTransaction()
{
    QSqlDatabase::database().transaction();
}

void PhotoDAO::endTransaction()
{
    QSqlDatabase::database().commit();
}

PhotoDAO::PhotoDAO(QObject* parent) :
    QObject(parent)
{
}

void PhotoDAO::setRating(const QList<Photo>& list, int rating)
{
    QSqlQuery q;
    QString   query = "update photo set rating=:rating where id in (:id)";

    QString   ids;
    Photo     info;

    foreach(info, list)
    {
        ids += QString::number(info.id()) + ",";
        info.setRating(rating);
    }
    ids.chop(1);
    query.replace(":id", ids);
    q.prepare(query);
    q.bindValue(":rating", rating);
    q.exec();
}

void PhotoDAO::setFlag(const QList<Photo>& list, Photo::Flag flag)
{
    QSqlQuery q;
    QString   query = ("update photo set flag=:flag where id in (:id)");

    QString   ids;
    Photo     info;

    foreach(info, list)
    {
        ids += QString::number(info.id()) + ",";
        info.setFlag(flag);
    }
    ids.chop(1);
    query.replace(":id", ids);
    q.prepare(query);
    q.bindValue(":flag", (int)flag);
    q.exec();
}

void PhotoDAO::setColorLabel(const QList<Photo>& list,
    Photo::ColorLabel color)
{
    QSqlQuery q;
    QString   query = ("update photo set color=:color where id in (:id)");

    QString   ids;
    Photo     info;

    foreach(info, list)
    {
        ids += QString::number(info.id()) + ",";
        info.setColorLabel(color);
    }
    ids.chop(1);
    query.replace(":id", ids);
    q.prepare(query);
    q.bindValue(":color", (int)color);
    q.exec();
}

void PhotoDAO::insertKeywords(const QStringList& words)
{
    if (words.size() == 0)
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
    QSqlDatabase::database().transaction();
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
    QSqlDatabase::database().commit();
    rebuildKeywordTree(parent, 1);
}

void PhotoDAO::assignKeywords(const QStringList& words, const QList<Photo>& list)
{
    QSqlQuery q;
    QString   word;
    QSqlDatabase::database().transaction();

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
    QSqlDatabase::database().commit();
}

void PhotoDAO::removeKeywordsExcept(const QStringList& words, const QList<Photo>& list)
{
    QSqlQuery q;

    QString   query =
        "delete from photo_keyword \
            where photo_id in (:photo_id) \
            and keyword_id not in  \
            ( select id from keyword where keyword in (':keywords'))";

    Photo   info;
    QString photo_id;

    foreach(info, list)
    {
        photo_id += QString::number(info.id()) + ",";
    }
    photo_id.chop(1);
    QString keywords = words.join("','");

    query.replace(":photo_id", photo_id);
    query.replace(":keywords", keywords);

    if (!q.exec(query))
    {
        qDebug() << q.lastError();
        qDebug() << q.lastQuery();
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
    QString   photo_ids;
    Photo     info;

    foreach(info, list)
    {
        photo_ids += QString::number(info.id()) + ",";
    }
    photo_ids.chop(1);
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

QString PhotoDAO::joinIds(const QList<long long>& idList) const
{
    QString photoids;

    foreach(long long id, idList)
    {
        photoids += QString::number(id) + ",";
    }
    photoids.chop(1);
    return photoids;
}

QList<Photo> PhotoDAO::getPhotosById(QList<long long> idList) const
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

void PhotoDAO::filterList(QList<long long>& list, long long rootPathId, bool includeSubDirs) const
{
    QString query;

    // get the list of Id's that fall under the rootPath
    if (includeSubDirs)
    {
        // get left and right of the path root
        long long lft, rgt;

        getLeftRightForPathId(rootPathId, lft, rgt);

        query =
            "select p.id from photo p join (select child.* from path child join path ancestor \
            on child.lft >= ancestor.lft \
            and child.lft <= ancestor.rgt \
            group by child.lft order by child.lft) as d on p.path_id = d.id \
            and d.lft between :lft and :rgt and p.id in (:idList);";
        query.replace(":lft", QString::number(lft));
        query.replace(":rgt", QString::number(rgt));
    }
    else
    {
        query =
            "select p.id from photo p join (select child.* from path child join path ancestor \
                on child.lft >= ancestor.lft \
                and child.lft <= ancestor.rgt \
                group by child.lft order by child.lft) as d on p.path_id = d.id \
                and p.path_id = :pathid and p.id in (:idList);";
        query.replace(":pathid", QString::number(rootPathId));
    }
    QString photoids = joinIds(list);
    query.replace(":idList", photoids);

    QSqlQuery q;

    if (!q.exec(query) && !q.isValid())
    {
        qDebug() << "Query failed" << q.executedQuery();
        qDebug() << q.lastError();
        return;
    }

    QList<long long> newList;

    while (q.next())
    {
        newList << q.value(0).toLongLong();
    }
    list = newList;
}

void PhotoDAO::deletePhotos(const QList<Photo>& list, bool deleteFile)
{
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

void PhotoDAO::updateExifInfo(const Photo& photo) const
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

    return;
}

void PhotoDAO::regenerateHash(Photo& p)
{
    QString   path = p.srcImagePath();
    // only scan the first 1MB
    long long hash = computeImageFileHash(path);

    p.setHash(hash);

    QSqlQuery q;
    q.prepare("update photo set photo_hash=:hash where id = :photoid");
    q.bindValue(":hash", p.hash());
    q.bindValue(":photoid", p.id());

    if (!q.exec())
    {
        qDebug() << "Query failed" << q.executedQuery();
        qDebug() << q.lastError();
    }

    return;
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
