#ifndef PHOTOSTAGE_DBUTILS_H
#define PHOTOSTAGE_DBUTILS_H

#include <QSqlQuery>
#include <QDateTime>
#include <QString>
#include <QVariant>

#include "nullable.h"
#include "photo.h"

namespace PhotoStage
{
QVariant setDbValue(const Nullable<uint8_t>& n);
QVariant setDbValue(const Nullable<float>& n);
QVariant setDbValue(const Nullable<double>& n);
QVariant setDbValue(const Nullable<QDateTime>& n);
QVariant setDbValue(const Nullable<QString>& n);
QVariant setDbValue(const Nullable<bool>& n);

void getDbValue(QSqlQuery& q, int col, Nullable<uint8_t>& out );
void getDbValue(QSqlQuery& q, int col, Nullable<float>& out);
void getDbValue(QSqlQuery& q, int col, Nullable<QDateTime>& out);
void getDbValue(QSqlQuery& q, int col, Nullable<QString>& out);
void getDbValue(QSqlQuery& q, int col, Nullable<bool>& out);

QString joinIds(const QList<long long>& idList);
QString joinIds(const QList<Photo>& idList);
}

#endif // PHOTOSTAGE_DBUTILS_H
