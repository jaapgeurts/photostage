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

void PhotoWorkUnit::setRating(long long id, int rating)
{

    QSqlQuery q;
    q.prepare("update photo set rating=:rating where id=:id");
    q.bindValue(":rating",rating);
    q.bindValue(":id",id);
    q.exec();
}

void PhotoWorkUnit::insertKeywords(const QStringList &words)
{
    // TODO sql: set unique index and ingore error when inserting
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
}

void PhotoWorkUnit::assignKeywords(const QStringList &words, const SqlPhotoInfo &info)
{
    QSqlQuery q;
    QString word;
    q.prepare("insert into photo_keyword (photo_id, keyword_id) select :photo_id, k.id from keyword k where k.keyword = :keyword");
    q.bindValue(":photo_id",info.id);
    foreach(word, words)
    {
        q.bindValue(":keyword",word);
        q.exec();
    }
}

QStringList PhotoWorkUnit::getPhotoKeywords(const SqlPhotoInfo &info)
{
    QSqlQuery q;
    q.prepare("select k.keyword from keyword k, photo_keyword pk on k.id = pk.keyword_id where pk.photo_id = :photo_id ");
    q.bindValue(":photo_id",info.id);
    if (!q.exec())
    {
        qDebug() << q.lastError();
        qDebug() << q.lastQuery();
    }

    QStringList list;
    while (q.next())
    {
        list.append(q.value(0).toString());
    }
    return list;
}

