#ifndef PHOTOSTAGE_HISTORYMODEL_H
#define PHOTOSTAGE_HISTORYMODEL_H

#include <QAbstractListModel>

#include "database/developitems.h"
#include "photo.h"

namespace PhotoStage
{
class HistoryModel : public QAbstractListModel
{
    public:

        HistoryModel(QWidget* parent = 0);

        void setPhoto(const Photo& photo);

        int rowCount(const QModelIndex& parent) const;
        QVariant data(const QModelIndex& index, int role) const;

    private:

        QSharedPointer<DevelopHistory> mDevelopHistory;
        Photo                          mPhoto;
};
}
#endif // PHOTOSTAGE_HISTORYMODEL_H
