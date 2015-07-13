#ifndef MAP_H
#define MAP_H

#include <QAbstractItemModel>
#include <QItemSelectionModel>

#include "module.h"
#include "photo.h"

#include "widgets/mapview/mapprovider.h"
#include "widgets/mapview/layer.h"

namespace Ui
{
class Map;
}

namespace PhotoStage
{
class Map : public Module
{
    Q_OBJECT

    public:

        explicit Map(QAbstractItemModel* model, QWidget* parent = 0);
        ~Map();

        QRect lightGap();

        void setSelectionModel(QItemSelectionModel* selectionModel);

    protected:

        void showEvent(QShowEvent*);

    private slots:

        void onCurrentPhotoChanged(const QModelIndex& current,
            const QModelIndex&);
        void onSelectionChanged(const QItemSelection& selected,
            const QItemSelection& deselected);

        void onModelReset();

    private:

        Ui::Map*              ui;

        QAbstractItemModel*   mPhotoModel;
        MapView::MapProvider* mMapProvider;
        MapView::Layer*       mLayer;
        bool                  mLoadPhoto;
        Photo                 mPhoto;

        void setPhoto(Photo photo);
};
}
#endif // MAP_H
