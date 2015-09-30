#ifndef PHOTOSTAGE_DEVELOPSETTINGDAO_H
#define PHOTOSTAGE_DEVELOPSETTINGDAO_H

#include <QObject>
#include <QList>
#include <QHash>

#include "developitems.h"
#include "engine/developrawparameters.h"
#include "nullable.h"
#include "photo.h"

namespace PhotoStage
{
class DatabaseAccess;

class DevelopSettingDao : public QObject
{
    friend class DatabaseAccess;

    Q_OBJECT

    public:

        DevelopHistory getDevelopHistory(const Photo& photo) const;

        long long insertDefaultRawSettings(long long photoId, const DevelopRawParameters& params);
        long long insertHistory(long long moduleId, long long photoId, const QString& text, bool isMutable);

        DevelopRawParameters getRawSettings(long long photoId);

    private:

        DevelopSettingDao(QObject* parent = 0);

        QHash<long long, DevelopRawParameters> getRawHistoryForPhoto(long long id) const;
};
}
#endif // PHOTOSTAGE_DEVELOPSETTINGDAO_H
