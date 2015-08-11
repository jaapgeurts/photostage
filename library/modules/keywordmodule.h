#ifndef PHOTOSTAGE_KEYWORDMODULE_H
#define PHOTOSTAGE_KEYWORDMODULE_H

#include "librarymodule.h"
#include "widgets/fixedtreeview.h"
#include "keywordmodel.h"

namespace PhotoStage
{
class KeywordModule : public LibraryModule
{
    public:

        KeywordModule(QWidget* parent = 0);

    private:

        Widgets::FixedTreeView* mTrvwKeywords;
        KeywordModel*           mKeywordModel;
};
}
#endif // PHOTOSTAGE_KEYWORDMODULE_H
