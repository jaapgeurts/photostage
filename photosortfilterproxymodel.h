#ifndef PHOTOSTAGE_PHOTOSORTFILTERPROXYMODEL_H
#define PHOTOSTAGE_PHOTOSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QHash>

namespace PhotoStage
{
class PhotoSortFilterProxyModel : public QSortFilterProxyModel
{
    public:

        PhotoSortFilterProxyModel(QObject* parent = 0);

        void setFilter(const QString& filter);

    protected:

        bool filterAcceptsRow(int source_row,
            const QModelIndex& source_parent) const;

    private:

        QHash<QString, bool> mFilter;
};
}
#endif // PHOTOSTAGE_PHOTOSORTFILTERPROXYMODEL_H
