#include "dbutils.h"

namespace PhotoStage {
QVariant setDbValue(const Nullable<uint8_t>& n)
{
  if (n == nullptr)
    return QVariant(QVariant::Int);
  return QVariant(*n);
}

QVariant setDbValue(const Nullable<float>& n)
{
  if (n == nullptr)
    return QVariant(QVariant::Double);
  return QVariant(*n);
}

QVariant setDbValue(const Nullable<double>& n)
{
  if (n == nullptr)
    return QVariant(QVariant::Double);
  return QVariant(*n);
}

QVariant setDbValue(const Nullable<QDateTime>& n)
{
  if (n == nullptr)
    return QVariant(QVariant::DateTime);
  return QVariant(*n);
}

QVariant setDbValue(const Nullable<QString>& n)
{
  if (n == nullptr)
    return QVariant(QVariant::String);
  return QVariant(*n);
}

QVariant setDbValue(const Nullable<bool>& n)
{
  if (n == nullptr)
    return QVariant(QVariant::Bool);
  return QVariant(*n);
}

void getNullableValue(QSqlQuery& q, int col, Nullable<uint8_t>& out)
{
  out =
      q.value(col).isNull() ? nullptr : Nullable<uint8_t>(q.value(col).toInt());
}

void getNullableValue(QSqlQuery& q, int col, Nullable<float>& out)
{
  out =
      q.value(col).isNull() ? nullptr : Nullable<float>(q.value(col).toFloat());
}

void getNullableValue(QSqlQuery& q, int col, Nullable<QDateTime>& out)
{
  out = q.value(col).isNull() ? nullptr
                              : Nullable<QDateTime>(q.value(col).toDateTime());
}

void getNullableValue(QSqlQuery& q, int col, Nullable<QString>& out)
{
  out = q.value(col).isNull() ? nullptr
                              : Nullable<QString>(q.value(col).toString());
}

void getNullableValue(QSqlQuery& q, int col, Nullable<bool>& out)
{
  out = q.value(col).isNull() ? nullptr : Nullable<bool>(q.value(col).toBool());
}

void getNullableValue(QSqlQuery& q, int col, Nullable<long long>& out)
{
  out = q.value(col).isNull() ? nullptr
                              : Nullable<long long>(q.value(col).toLongLong());
}

QString joinIds(const QList<long long>& idList)
{
  QString photoids;

  foreach (long long id, idList)
  {
    photoids += QString::number(id) + ",";
  }
  photoids.chop(1);
  return photoids;
}

QString joinIds(const QList<Photo>& idList)
{
  QString ids;

  foreach (Photo info, idList)
  {
    ids += QString::number(info.id()) + ",";
  }
  ids.chop(1);
  return ids;
}
} // namespace PhotoStage
