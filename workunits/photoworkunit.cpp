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
        "with recursive cte as ( \
                            select id,directory \
                            from path \
                            where parent_id is NULL \
                            union all \
                            select t.id as id , cte.directory || :separator || t.directory as directory \
                            from cte, path t \
                            where cte.id = t.parent_id \
            ) select p.id, p.filename, c.directory,p.rating,p.color,p.flag, \
              p.iso, p.aperture, p.exposure_time, p.focal_length, p.datetime_original, \
              p.datetime_digitized, p.rotation, p.lattitude, p.longitude, \
              p.copyright, p.artist, p.flash, p.lens_name, p.make, p.model \
            from cte c, photo p \
            where c.id = p.path_id \
            and p.id in (:photoids) \
            order by p.id");

    QString photoids;

    for (int i = 0; i < idList.size(); i++)
    {
        photoids += QString::number(idList.at(i)) + ",";
    }
    photoids.chop(1);
    query.replace(":photoids", photoids);
    q.prepare(query);
    q.bindValue(":separator", QDir::separator());

    QList<Photo> photoInfoList;

    if (!q.exec())
    {
        qDebug() << "PhotoWorkUnit::getPhotosById() error" << q.lastError();
        return photoInfoList;
    }

    while (q.next())
    {
        photoInfoList.append(Photo(q));
    }
    return photoInfoList;
}

// TODO: make path_id work and option to include
QList<Photo> PhotoWorkUnit::getPhotosByPath(long long path_id,
    bool includeSubDirs)
{
    QSqlQuery q;

    QString   query;

    query =
        "WITH RECURSIVE cte AS ( \
            SELECT id,directory \
            FROM path \
            WHERE parent_id IS NULL \
            UNION ALL \
            SELECT t.id AS id , cte.directory || '/' || t.directory AS directory \
            FROM cte, path t \
            WHERE cte.id = t.parent_id ) \
            SELECT c.directory \
            FROM cte c \
            where c.id = :pathid";

    q.prepare(query);
    q.bindValue(":pathid", path_id);

    if (!q.exec())
        qDebug() << "Error getting path for selected id\n"  <<
            q.lastError();

    if (!q.first())
        qDebug() << "Query to get path didn't return results\n" <<
            q.lastError();
    QString path                 = q.value(0).toString();
    QString includeSubDirsClause = "c.directory like ':path%'";
    includeSubDirsClause.replace(":path", path);

    query = QString(
        "with recursive cte as ( \
                    select id,directory \
                    from path \
                    where parent_id is null \
                    union all \
                    select t.id as id , cte.directory || :separator || t.directory as directory \
                    from cte, path t \
                    where cte.id = t.parent_id \
            ) select p.id, p.filename, c.directory,p.rating,p.color,p.flag, \
              p.iso, p.aperture,p.exposure_time, p.focal_length, p.datetime_original, \
              p.datetime_digitized, p.rotation, p.lattitude,p.longitude, \
              p.copyright, p.artist, p.flash, p.lens_name, p.make,\
              p.model, c.id \
            from cte c, photo p \
            where c.id = p.path_id and :top_or_include \
            order by p.id");

    if (includeSubDirs)
        query.replace(":top_or_include", includeSubDirsClause);
    else
        query.replace(":top_or_include", "c.id = " + QString::number(
                path_id));

    q.clear();
    q.prepare(query);
    q.bindValue(":separator", QDir::separator());
    q.bindValue(":topid", path_id);

    if (!q.exec())
        qDebug() << "SqlPhotoModel error" << q.lastError();

    //qDebug() << q.lastQuery();

    QList<Photo> photoInfoList;

    while (q.next())
    {
        photoInfoList.append(Photo(q));
    }
    return photoInfoList;
}
}
