#ifndef PHOTOSTAGE_DRAGDROPINFO_H
#define PHOTOSTAGE_DRAGDROPINFO_H

#include <QList>

namespace PhotoStage
{
class DragDropInfo
{
    public:

        enum DragSourceModel
        {
            PathModel       = 1,
            PhotoModel      = 2,
            CollectionModel = 3
        };

        DragDropInfo(DragSourceModel source, const QList<long long>& idList);
        DragDropInfo(const QByteArray& data);

        QByteArray toByteArray();

        DragSourceModel sourceModel();
        const QList<long long>& idList();

    private:

        DragSourceModel  mSourceModel;
        QList<long long> mIdList;
};
}
#endif // PHOTOSTAGE_DRAGDROPINFO_H
