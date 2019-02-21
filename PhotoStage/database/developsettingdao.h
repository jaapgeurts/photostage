#ifndef PHOTOSTAGE_DEVELOPSETTINGDAO_H
#define PHOTOSTAGE_DEVELOPSETTINGDAO_H

#include <QHash>
#include <QList>
#include <QObject>

#include "developitems.h"
#include "engine/developrawparameters.h"
#include "nullable.h"
#include "photo.h"

namespace PhotoStage {
class DatabaseAccess;

class DevelopSettingDao : public QObject
{
  friend class DatabaseAccess;

  Q_OBJECT

public:
  QSharedPointer<DevelopHistory> getDevelopHistory(const Photo& photo) const;

  long long
  insertRawSettings(long long                                   photoId,
                    const QSharedPointer<DevelopRawParameters>& params,
                    const QString&                              description);
  long long
            insertDefaultRawSettings(long long                                   photoId,
                                     const QSharedPointer<DevelopRawParameters>& params);
  long long insertHistory(long long moduleId, long long photoId,
                          const QString& text, bool isMutable);

  QSharedPointer<DevelopRawParameters> getRawSettings(long long photoId);

private:
  DevelopSettingDao(QObject* parent = 0);

  QHash<long long, QSharedPointer<DevelopRawParameters>>
  getRawHistoryForPhoto(long long id) const;
};
} // namespace PhotoStage
#endif // PHOTOSTAGE_DEVELOPSETTINGDAO_H
