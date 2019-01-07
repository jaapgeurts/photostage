#include <QtSql>
#include <QSqlQuery>
#include <QDebug>

#include "developsettingdao.h"
#include "engine/developrawparameters.h"
#include "constants.h"

namespace PhotoStage
{
QSharedPointer<DevelopHistory> DevelopSettingDao::getDevelopHistory(const Photo& photo) const
{
    QSqlQuery q;
    QString   query =
        "select id,develop_module_id, friendlytext, is_mutable from develop_history where photo_id = :photoid;";

    q.prepare(query);
    q.bindValue(":photoid", photo.id());

    if (!q.exec())
    {
        qDebug() << q.lastError();
    }

    QList<QSharedPointer<DevelopHistoryItem> > list;

    // get all RawHistory for this photo
    QHash<long long, QSharedPointer<DevelopRawParameters> > rawsettings = getRawHistoryForPhoto(photo.id());

    while (q.next())
    {
        QSharedPointer<DevelopHistoryItem> item = QSharedPointer<DevelopHistoryItem>(
            new DevelopHistoryItem(q.value(0).toLongLong(), q.value(1).toLongLong(),
            q.value(2).toString(), q.value(3).toBool()));

        switch (item->moduleType)
        {
            case DEVELOP_SETTINGS_RAW:

                if (rawsettings.contains(item->id))
                    item->developItem = rawsettings.value(item->id);
                break;
        }

        list.append(item);
    }

    return QSharedPointer<DevelopHistory>( new DevelopHistory(list));
}

QSharedPointer<DevelopRawParameters> DevelopSettingDao::getRawSettings(long long photoId)
{
    QSqlQuery q;
    QString   query = "select dh.id, develop_history_id, red_multiplier, green_multiplier, blue_multiplier, "
        " rotation, demosaic_algorithm "
        " from develop_setting_raw dsr join develop_history dh on dsr.develop_history_id = dh.id "
        " where photo_id = :photoid and develop_module_id = :moduleid order by dh.id desc";

    q.prepare(query);
    q.bindValue(":photoid", photoId);
    q.bindValue(":moduleid", DEVELOP_SETTINGS_RAW);

    if (!q.exec())
    {
        qDebug() << q.lastError();
        return QSharedPointer<DevelopRawParameters>();
    }

    if (q.next())
    {
        QSharedPointer<DevelopRawParameters> param = QSharedPointer<DevelopRawParameters>(
            new DevelopRawParameters(q.value(0).toLongLong(), q.value(2).toFloat(),
            q.value(3).toFloat(), q.value(4).toFloat()));
        param->rotation  = (DevelopRawParameters::Rotation)q.value(5).toInt();
        param->algorithm = (DevelopRawParameters::InterpolationAlgorithm)q.value(5).toInt();
        return param;
    }
    return QSharedPointer<DevelopRawParameters>();
}

long long DevelopSettingDao::insertHistory(long long moduleId, long long photoId, const QString& text, bool isMutable)
{
    QSqlQuery q;
    QString   query =
        "insert into develop_history (develop_module_id,photo_id,friendlytext,is_mutable) "
        " values ( :moduleid, :photoid, :friendlytext, :mutable )";

    q.prepare(query);
    q.bindValue(":moduleid", moduleId);
    q.bindValue(":photoid", photoId);
    q.bindValue(":friendlytext", text);
    q.bindValue(":mutable", isMutable);

    if (!q.exec())
    {
        qDebug() << q.lastError();
        return -1;
    }

    return q.lastInsertId().toLongLong();
}

long long DevelopSettingDao::insertDefaultRawSettings(long long photoId,
    const QSharedPointer<DevelopRawParameters>& params)
{
    long long id = insertHistory(DEVELOP_SETTINGS_RAW, photoId, "Basic RAW Settings", false);

    QSqlQuery q;
    QString   query =
        "insert into  develop_setting_raw ( develop_history_id, red_multiplier, green_multiplier, blue_multiplier, "
        " rotation, demosaic_algorithm ) values ( :historyid, :red, :green, :blue, :rotation, :algorithm)";

    q.prepare(query);
    q.bindValue(":historyid", id);
    q.bindValue(":red", params->redMultiplier);
    q.bindValue(":green", params->greenMultiplier);
    q.bindValue(":blue", params->blueMultiplier);
    q.bindValue(":rotation", (int)params->rotation);
    q.bindValue(":algorithm", (int)params->algorithm);

    if (!q.exec())
    {
        qDebug() << q.lastError();
        return -1;
    }

    return q.lastInsertId().toLongLong();
}

long long DevelopSettingDao::insertRawSettings(long long photoId,
    const QSharedPointer<DevelopRawParameters>& params,
    const QString& description)
{
    long long id = insertHistory(DEVELOP_SETTINGS_RAW, photoId, description, true);

    QSqlQuery q;
    QString   query =
        "insert into  develop_setting_raw ( develop_history_id, red_multiplier, green_multiplier, blue_multiplier, "
        " rotation, demosaic_algorithm ) values ( :historyid, :red, :green, :blue, :rotation, :algorithm)";

    q.prepare(query);
    q.bindValue(":historyid", id);
    q.bindValue(":red", params->redMultiplier);
    q.bindValue(":green", params->greenMultiplier);
    q.bindValue(":blue", params->blueMultiplier);
    q.bindValue(":rotation", (int)params->rotation);
    q.bindValue(":algorithm", (int)params->algorithm);

    if (!q.exec())
    {
        qDebug() << q.lastError();
        return -1;
    }

    return q.lastInsertId().toLongLong();
}

QHash<long long, QSharedPointer<DevelopRawParameters> > DevelopSettingDao::getRawHistoryForPhoto(long long id) const
{
    QHash<long long, QSharedPointer<DevelopRawParameters> > hash;

    // other
    QSqlQuery q;
    QString   query = "select dh.id, develop_history_id, red_multiplier, green_multiplier, blue_multiplier, "
        " rotation, demosaic_algorithm "
        " from develop_setting_raw dsr join develop_history dh on dsr.develop_history_id = dh.id "
        " where photo_id = :photoid order by dh.id asc";

    q.prepare(query);
    q.bindValue(":photoid", id);

    if (!q.exec())
    {
        qDebug() << q.lastError();
        return hash;
    }

    while (q.next())
    {
        QSharedPointer<DevelopRawParameters> param = QSharedPointer<DevelopRawParameters>(
            new DevelopRawParameters(q.value(0).toLongLong(), q.value(2).toFloat(),
            q.value(3).toFloat(), q.value(4).toFloat()));
        param->rotation  = (DevelopRawParameters::Rotation)q.value(5).toInt();
        param->algorithm = (DevelopRawParameters::InterpolationAlgorithm)q.value(5).toInt();
        hash.insert(q.value(1).toLongLong(), param);
    }

    return hash;
}

DevelopSettingDao::DevelopSettingDao(QObject* parent) :
    QObject(parent)
{
}
}
