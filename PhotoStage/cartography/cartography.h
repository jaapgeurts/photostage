#ifndef PHOTOSTAGE_MAP_H
#define PHOTOSTAGE_MAP_H

#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QMenu>
#include <QSettings>

#include "module.h"
#include "photo.h"

#include "widgets/mapview/mapprovider.h"
#include "widgets/mapview/modelindexlayer.h"

namespace Ui {
class Map;
}

namespace PhotoStage {
class Cartography : public Module
{
  Q_OBJECT

public:
  explicit Cartography(QAbstractItemModel* model, QWidget* parent = 0);
  ~Cartography();

  QRect lightGap();

  void setSelectionModel(QItemSelectionModel* selectionModel);
  void setModel(QAbstractItemModel* model);

protected:
  void showEvent(QShowEvent*);

private slots:

  void onCurrentPhotoChanged(const QModelIndex& current, const QModelIndex&);
  void onSelectionChanged(const QItemSelection&, const QItemSelection&);
  void onZoomLevelChanged(int level);
  void onCenterChanged(const QGeoCoordinate& coord);
  void onContextMenuRequested(const QGeoCoordinate& coord, const QPoint& pos);

  void onAssignPhotos();

private:
  Ui::Map* ui;

  QAbstractItemModel*       mPhotoModel;
  MapView::MapProvider*     mMapProvider;
  MapView::ModelIndexLayer* mLayer;
  bool                      mLoadPhoto;
  Photo                     mPhoto;
  QItemSelectionModel*      mSelectionModel;
  QGeoCoordinate            mSavedCoordinate;
  int                       mZoomLevel;
  QMenu*                    mContextMenu;
  QGeoCoordinate            mSelectedCoordinate;

  void setPhoto(Photo photo);
};
} // namespace PhotoStage
#endif // MAP_H
