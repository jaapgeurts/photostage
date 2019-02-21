#ifndef PHOTOSTAGE_KEYWORDDAO_H
#define PHOTOSTAGE_KEYWORDDAO_H

#include <QMap>
#include <QStringList>

#include "keyworditem.h"
#include "photo.h"

namespace PhotoStage {
class DatabaseAccess;

class KeywordDAO : public QObject
{
  friend class DatabaseAccess;

  Q_OBJECT

public:
  KeywordItem* getKeywordItems();
  void         deleteKeywordItems(KeywordItem* root);

  void insertKeywords(const QStringList& words);
  void assignKeywords(const QStringList& words, const QList<Photo>& list);
  void unAssignKeywordsExcept(const QStringList&  words,
                              const QList<Photo>& list);

  QMap<QString, int> getPhotoKeywordsCount(const QList<Photo>& list) const;
  QStringList        getPhotoKeywords(const Photo& photo) const;

signals:

  void keywordsAdded();
  void keywordsDeleted();
  void keywordsAssignmentChanged(const QList<Photo>& photos);

public slots:

private:
  explicit KeywordDAO(QObject* parent = 0);

  void      getKeywordItemsRec(KeywordItem* root);
  long long rebuildKeywordTree(long long parent_id, long long left);
};
} // namespace PhotoStage
#endif // PHOTOSTAGE_KEYWORDDAO_H
