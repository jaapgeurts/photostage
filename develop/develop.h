#ifndef DEVELOP_H
#define DEVELOP_H

#include <QItemSelectionModel>
#include <QModelIndex>

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
        QItemSelectionModel*    mPhotoModel;

        void doSetPhoto(Photo photo);
};
}

#endif // DEVELOP_H
