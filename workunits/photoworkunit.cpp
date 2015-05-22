#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

#include "photoworkunit.h"

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

void PhotoWorkUnit::setRating(const QList<SqlPhotoInfo> & list, int rating)
{

    QSqlQuery q;
    q.prepare("update photo set rating=:rating where id in (:id)");
    q.bindValue(":rating",rating);

    QString ids;
    SqlPhotoInfo info;
    foreach(info, list)
        ids += QString::number(info.id) + ",";
    ids.chop(1);
    q.bindValue(":id",ids);
    q.exec();
}

void PhotoWorkUnit::insertKeywords(const QStringList &words)
{
    if (words.size() == 0 )
        return;

    QSqlQuery q;
    QStringList newWords = words;
    q.prepare("select id, keyword from keyword where keyword in (:list)");
    QString list = words.join("','");
    q.bindValue(":list",list);
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
    else {
        qDebug() << "no root item for keyword";
        return;
    }

    QSqlDatabase::database().transaction();
    q.clear();
    q.prepare("insert into keyword (keyword,parent_id) values ( :keyword , :parent )");
    q.bindValue(":parent",parent);
    QString word;
    foreach(word, newWords)
    {
        q.bindValue(":keyword",word);
        if (!q.exec())
            qDebug() << q.lastError();
    }
    QSqlDatabase::database().commit();
}

void PhotoWorkUnit::assignKeywords(const QStringList &words,const QList<SqlPhotoInfo> &list)
{
    QSqlQuery q;
    QString word;
    QSqlDatabase::database().transaction();
    q.prepare("insert into photo_keyword (photo_id, keyword_id) "\
              "select :photo_id, k.id "\
              "from keyword k "\
              " where k.keyword = :keyword");
    SqlPhotoInfo info;
    foreach(info, list)
    {
        q.bindValue(":photo_id",info.id);
        foreach(word, words)
        {
            q.bindValue(":keyword",word);
            q.exec();
        }
    }
    QSqlDatabase::database().commit();
}

void PhotoWorkUnit::removeKeywordsExcept(const QStringList &words, const QList<SqlPhotoInfo> &list)
{
    QSqlQuery q;

    QString query = "delete from photo_keyword "\
              "where photo_id in (:photo_id) "\
              " and keyword_id not in " \
              " ( select id from keyword where keyword in (':keywords'))";

    SqlPhotoInfo info;
    QString photo_id;
    foreach(info, list)
    {
        photo_id += QString::number(info.id) + ",";
    }
    photo_id.chop(1);
    QString keywords = words.join("','");

    query.replace(":photo_id",photo_id);
    query.replace(":keywords",keywords);

    if (!q.exec(query))
    {
        qDebug() << q.lastError();
        qDebug() << q.lastQuery();
    }
}

QMap<QString,int> PhotoWorkUnit::getPhotoKeywords(const QList<SqlPhotoInfo> & list) const
{
    QSqlQuery q;
    QString query = "select k.keyword, count(pk.photo_id) " \
                    "from keyword k, photo_keyword pk on k.id = pk.keyword_id " \
                    " where pk.photo_id in (:photo_ids) " \
                    " group by k.keyword order by k.keyword ";
    QString photo_ids;
    SqlPhotoInfo info;
    foreach(info, list)
        photo_ids += QString::number(info.id) + ",";
    photo_ids.chop(1);
    query.replace(":photo_ids",photo_ids);

    if (!q.exec(query))
    {
        qDebug() << q.lastError();
        qDebug() << q.lastQuery();
    }

    QMap<QString,int> dict;
    while (q.next())
    {
        dict.insert(q.value(0).toString(), q.value(1).toInt());
    }
    return dict;
}

