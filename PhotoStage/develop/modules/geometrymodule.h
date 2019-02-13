#ifndef PHOTOSTAGE_GEOMETRYMODULE_H
#define PHOTOSTAGE_GEOMETRYMODULE_H

#include <QWidget>

#include "developmodule.h"

namespace Ui
{
class GeometryModule;
}

namespace PhotoStage
{
class GeometryModule : public DevelopModule
{
    Q_OBJECT

    public:

        GeometryModule(QWidget* parent = 0L);

    signals:

        void cropRotateClicked();
        void lockAspectRatioClicked(bool enabled);
        void aspectRatioChanged();

    private slots:

        void onCropRotateClicked();
        void onLockAspectClicked(bool enabled);
        void onAspectRatioChanged(int aspect);

    private:

        Ui::GeometryModule* ui;
};
}
#endif // PHOTOSTAGE_GEOMETRYMODULE_H
