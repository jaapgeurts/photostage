#ifndef PHOTOSTAGE_KEYWORDITEM_H
#define PHOTOSTAGE_KEYWORDITEM_H

#include <QList>
#include <QString>

namespace PhotoStage {
struct KeywordItem {
  KeywordItem(long long id, const QString& keyword, long long parent_id);

  QString                    keyword;
  long long                  id;
  long long                  parent_id;
  struct KeywordItem*        parent;
  QList<struct KeywordItem*> children;
};
} // namespace PhotoStage
#endif // PHOTOSTAGE_KEYWORDITEM_H
