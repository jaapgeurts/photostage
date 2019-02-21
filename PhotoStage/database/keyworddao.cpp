#include <QtSql>

#include "databaseaccess.h"
#include "dbutils.h"
#include "keyworddao.h"

namespace PhotoStage {
KeywordDAO::KeywordDAO(QObject* parent) : QObject(parent)
{
}

KeywordItem* KeywordDAO::getKeywordItems()
{
  // create table keyword
  // (id integer primary key AUTOINCREMENT,
  //   keyword varchar,
  //  parent_id integer

  // first create the root item
  QSqlQuery query;

  QString queryText = QString(
      "select id,keyword,parent_id from keyword where parent_id is NULL");

  if (!query.exec(queryText))
    qDebug() << query.lastError();

  KeywordItem* item = NULL;

  if (query.first())
  {
    item =
        new KeywordItem(query.value(0).toLongLong(), query.value(1).toString(),
                        query.value(2).toLongLong());
    getKeywordItemsRec(item);
  }
  return item;
}

void KeywordDAO::getKeywordItemsRec(KeywordItem* root)
{
  QSqlQuery query;
  QString queryText = QString("select id,keyword,parent_id from keyword where "
                              "parent_id = :parent_id order by keyword;");

  query.prepare(queryText);

  KeywordItem* item;

  query.bindValue(":parent_id", root->id);
  query.exec();

  while (query.next())
  {
    item =
        new KeywordItem(query.value(0).toLongLong(), query.value(1).toString(),
                        query.value(1).toLongLong());
    item->parent = root;
    root->children.append(item);
    getKeywordItemsRec(item);
  }
}

void KeywordDAO::deleteKeywordItems(KeywordItem* root)
{
  KeywordItem* item;

  foreach (item, root->children)
  {
    deleteKeywordItems(item);
  }
  root->children.clear();
  delete root;
}

void KeywordDAO::insertKeywords(const QStringList& words)
{
  if (words.isEmpty())
    return;

  // find keywords already in the database
  QSqlQuery   q;
  QStringList newWords = words;
  q.prepare("select id, keyword from keyword where keyword in (:list)");
  QString list = words.join("','");
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
  q.finish();
  q.prepare("select id from keyword where parent_id is NULL");
  q.exec();
  long long parent;

  if (q.first())
    parent = q.value(0).toLongLong();
  else
  {
    // insert root because there is none.
    q.finish();
    q.exec("insert into keyword (keyword,parent_id) values('',NULL) ");
    parent = q.lastInsertId().toLongLong();
  }

  // insert all the keywords.
  DatabaseAccess::instance()->beginTransaction();
  q.finish();
  q.prepare(
      "insert into keyword (keyword,parent_id) values ( :keyword , :parent )");
  q.bindValue(":parent", parent);
  QString word;
  foreach (word, newWords)
  {
    q.bindValue(":keyword", word);

    if (!q.exec())
      qDebug() << q.lastError();
  }
  // TODO: improve performance and don't rebuild tree on each insert.
  DatabaseAccess::instance()->endTransaction();
  q.finish();
  rebuildKeywordTree(parent, 1);
  emit keywordsAdded();
}

void KeywordDAO::assignKeywords(const QStringList&  words,
                                const QList<Photo>& list)
{
  if (list.isEmpty())
    return;

  if (words.isEmpty())
    return;

  QSqlQuery q;
  QString   word;
  DatabaseAccess::instance()->beginTransaction();

  q.prepare("insert into photo_keyword (photo_id, keyword_id) \
              select :photo_id, k.id \
              from keyword k \
              where k.keyword = :keyword");
  Photo info;
  foreach (info, list)
  {
    q.bindValue(":photo_id", info.id());
    foreach (word, words)
    {
      q.bindValue(":keyword", word);
      q.exec();
    }
  }
  DatabaseAccess::instance()->endTransaction();
  // TODO: consider photosChanged() vs keywordAssignmentsChanged()
  emit keywordsAssignmentChanged(list);
}

void KeywordDAO::unAssignKeywordsExcept(const QStringList&  words,
                                        const QList<Photo>& list)
{
  if (list.isEmpty())
    return;

  if (words.isEmpty())
    return;

  QSqlQuery q;

  QString query = "delete from photo_keyword \
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

  emit keywordsAssignmentChanged(list);
}

// return keywords for the selected photos, and the count each keyword appears
QMap<QString, int>
KeywordDAO::getPhotoKeywordsCount(const QList<Photo>& list) const
{
  QSqlQuery q;
  QString   query     = "select k.keyword, count(pk.photo_id) \
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

QStringList KeywordDAO::getPhotoKeywords(const Photo& photo) const
{
  QSqlQuery q;

  q.prepare("select k.keyword\
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

long long KeywordDAO::rebuildKeywordTree(long long parent_id, long long left)
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
    right        = rebuildKeywordTree(id, right);
  }

  q.prepare("update keyword set lft=:left, rgt=:right where id=:parent_id;");

  q.bindValue(":left", left);
  q.bindValue(":right", right);
  q.bindValue(":parent_id", parent_id);

  q.exec();

  // return the right value of this node + 1

  return right + 1;
}
} // namespace PhotoStage
