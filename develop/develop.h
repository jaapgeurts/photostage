#ifndef PHOTOSTAGE_DEVELOP_H
#define PHOTOSTAGE_DEVELOP_H

#include <QItemSelectionModel>
#include <QModelIndex>

#include "threadqueue.h"
#include "module.h"
#include "modules/develophistogrammodule.h"
#include "modules/rawmodule.h"
#include "modules/basicmodule.h"
#include "photo.h"

namespace Ui
{
class Develop;
}

namespace PhotoStage
{
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

    private:

        Ui::Develop*            ui;
        DevelopHistogramModule* mHistogramModule;
        RawModule*              mRawModule;
        BasicModule*            mBasicModule;
        Photo                   mPhoto;
        bool                    mLoadPhoto;
        DevelopHistory          mDevelopHistory;

        void doSetPhoto(Photo photo);
};
}

#endif // PHOTOSTAGE_DEVELOP_H
