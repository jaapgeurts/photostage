#ifndef PHOTOSTAGE_DBUTILS_H
#define PHOTOSTAGE_DBUTILS_H

#include <QDateTime>
#include <QSqlQuery>
#include <QString>
#include <QVariant>

#include "nullable.h"
#include "photo.h"

namespace PhotoStage {
QVariant setDbValue(const Nullable<uint8_t>& n);
QVariant setDbValue(const Nullable<float>& n);
QVariant setDbValue(const Nullable<double>& n);
QVariant setDbValue(const Nullable<QDateTime>& n);
QVariant setDbValue(const Nullable<QString>& n);
QVariant setDbValue(const Nullable<bool>& n);

void getNullableValue(QSqlQuery& q, int col, Nullable<uint8_t>& out);
void getNullableValue(QSqlQuery& q, int col, Nullable<float>& out);
void getNullableValue(QSqlQuery& q, int col, Nullable<QDateTime>& out);
void getNullableValue(QSqlQuery& q, int col, Nullable<QString>& out);
void getNullableValue(QSqlQuery& q, int col, Nullable<bool>& out);
void getNullableValue(QSqlQuery& q, int col, Nullable<long long>& out);

QString joinIds(const QList<long long>& idList);
QString joinIds(const QList<Photo>& idList);
} // namespace PhotoStage

#endif // PHOTOSTAGE_DBUTILS_H
