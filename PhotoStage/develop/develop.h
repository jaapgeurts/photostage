#ifndef PHOTOSTAGE_DEVELOP_H
#define PHOTOSTAGE_DEVELOP_H

#include <QItemSelectionModel>
#include <QModelIndex>

#include "module.h"
#include "modules/basicmodule.h"
#include "modules/develophistogrammodule.h"
#include "modules/geometrymodule.h"
#include "modules/historymodule.h"
#include "modules/rawmodule.h"
#include "photo.h"
#include "threadqueue.h"

namespace Ui {
class Develop;
}

namespace PhotoStage {
class Develop : public Module
{
  Q_OBJECT

public:
  explicit Develop(QWidget* parent = 0);
  ~Develop();

  QRect lightGap();

  void setPhoto(Photo photo);

public slots:

  void onPhotoUpdated();

protected:
  void showEvent(QShowEvent*);

private slots:

  void onDevelopSettingsChanged();
  void onCropRotateClicked();
  void onLockAspectRatioClicked(bool enabled);

private:
  Ui::Develop*            ui;
  DevelopHistogramModule* mHistogramModule;
  RawModule*              mRawModule;
  BasicModule*            mBasicModule;
  HistoryModule*          mHistoryModule;
  GeometryModule*         mGeometryModule;
  Photo                   mPhoto;
  bool                    mLoadPhoto;
  // DevelopHistory          mDevelopHistory;

  void doSetPhoto(Photo photo);
};
} // namespace PhotoStage

#endif // PHOTOSTAGE_DEVELOP_H
