#include <QDataStream>

#include "dragdropinfo.h"

namespace PhotoStage {
DragDropInfo::DragDropInfo(DragSourceModel         source,
                           const QList<long long>& idList)
    : mSourceModel(source), mIdList(idList)
{
}

DragDropInfo::DragDropInfo(const QByteArray& data)
{
  QDataStream in(data);

  qint32 val;

  in >> val;
  mSourceModel = (DragSourceModel)val;

  qint32 count;
  in >> count;

  for (int i = 0; i < count; i++)
  {
    long long id;
    in >> id;
    mIdList.append(id);
  }
}

QByteArray DragDropInfo::toByteArray()
{
  QByteArray  data;
  QDataStream out(&data, QIODevice::WriteOnly);

  out << (qint32)mSourceModel;
  out << (qint32)mIdList.size();

  foreach (long long id, mIdList)
  {
    out << id;
  }

  return data;
}

DragDropInfo::DragSourceModel DragDropInfo::sourceModel()
{
  return mSourceModel;
}

const QList<long long>& DragDropInfo::idList()
{
  return mIdList;
}
} // namespace PhotoStage
