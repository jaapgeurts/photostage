#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QDir>

#include "photoworkunit.h"

namespace PhotoStage
{
// static initializers
PhotoWorkUnit* PhotoWorkUnit::mInstance = NULL;

PhotoWorkUnit* PhotoWorkUnit::instance()
{
    if (mInstance == NULL)
        mInstance = new PhotoWorkUnit();
    return mInstance;
}

PhotoWorkUnit::PhotoWorkUnit()
{
}

void PhotoWorkUnit::setRating(const QList<Photo>& list, int rating)
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

void PhotoWorkUnit::setFlag(const QList<Photo>& list, Photo::Flag flag)
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

void PhotoWorkUnit::setColorLabel(const QList<Photo>& list,
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

void PhotoWorkUnit::insertKeywords(const QStringList& words)
{
    if (words.size() == 0)
        return;

    QSqlQuery   q;
    QStringList newWords = words;
    q.prepare("select id, keyword from keyword where keyword in (:list)");
    QString     list = words.join("','");
    q.bindValue(":list", list);
    q.exec();

    while (q.next())
    {
        newWords.removeAll(q.value(1).toString());
    }

    q.clear();
    q.prepare("select id from keyword where parent_id is NULL");
    q.exec();
    int parent;

    if (q.first())
        parent = q.value(0).toLongLong();
    else
    {
        qDebug() << "no root item for keyword";
        return;
    }

    QSqlDatabase::database().transaction();
    q.clear();
    q.prepare(
        "insert into keyword (keyword,parent_id) values ( :keyword , :parent )");
    q.bindValue(":parent", parent);
    QString word;
    foreach(word, newWords)
    {
        q.bindValue(":keyword", word);

        if (!q.exec())
            qDebug() << q.lastError();
    }
    QSqlDatabase::database().commit();
}

void PhotoWorkUnit::assignKeywords(const QStringList& words,
    const QList<Photo>& list)
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

void PhotoWorkUnit::removeKeywordsExcept(const QStringList& words,
    const QList<Photo>& list)
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

QMap<QString,
int> PhotoWorkUnit::getPhotoKeywords(const QList<Photo>& list) const
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
    photo_ids += QString::number(info.id()) + ",";
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

QList<Photo> PhotoWorkUnit::getPhotosById(QList<long long> idList)
{
    QSqlQuery q;
    QString   query = QString(
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
              and p.id in (:photoids);");

    QString photoids;

    for (int i = 0; i < idList.size(); i++)
    {
        photoids += QString::number(idList.at(i)) + ",";
    }
    photoids.chop(1);
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
        list.append(Photo(q));
    }
    return list;
}

void PhotoWorkUnit::updateExifInfo(const Photo& photo) const
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

// TODO: make path_id work and option to include
QList<Photo> PhotoWorkUnit::getPhotosByPath(long long path_id,
    bool includeSubDirs)
{
    QList<Photo> list;

    QSqlQuery    q;

    q.prepare("select lft,rgt from path where id = :id;");
    q.bindValue(":id", path_id);

    if (!q.exec() && !q.isValid())
    {
        qDebug() << "Query failed" << q.executedQuery();
        qDebug() << q.lastError();
        return list;
    }
    q.next();

    long long lft = q.value(0).toLongLong();
    long long rgt = q.value(1).toLongLong();

    //    if (includeSubDirs)
    //        query.replace(":top_or_include", includeSubDirsClause);
    //    else
    //        query.replace(":top_or_include", "c.id = " + QString::number(
    //                path_id));

    q.clear();

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
    q.bindValue(":separator", QDir::separator());
    q.bindValue(":lft", lft);
    q.bindValue(":rgt", rgt);

    if (!q.exec() && !q.isValid())
    {
        qDebug() << "Query failed" << q.executedQuery();
        qDebug() << q.lastError();
        return list;
    }

    while (q.next())
    {
        list.append(Photo(q));
    }
    return list;
}
}
