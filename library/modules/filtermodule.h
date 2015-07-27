#ifndef PHOTOSTAGE_FILTERMODULE_H
#define PHOTOSTAGE_FILTERMODULE_H

#include "librarymodule.h"
#include "photofilterinfo.h"

namespace Ui
{
class FilterModule;
}

namespace PhotoStage
{
class FilterModule : public LibraryModule
{
    Q_OBJECT

    public:

        FilterModule(QWidget* parent = 0);

    public slots:

        void checkFlags();

        void checkColors();

        void onRating0Clicked();
        void onRating1Clicked();
        void onRating2Clicked();
        void onRating3Clicked();
        void onRating4Clicked();
        void onRating5Clicked();

        void onKeywordsEntered();
        void onKeywordsToggled(bool state);

    signals:

        void modelFilterApplied(const PhotoFilterInfo& filter);

    private:

        Ui::FilterModule* ui;
        PhotoFilterInfo   mFilterInfo;
        QString           mOldText;

        void parseKeywords();
};
}
#endif // PHOTOSTAGE_FILTERMODULE_H
