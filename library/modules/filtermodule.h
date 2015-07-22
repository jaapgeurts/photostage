#ifndef FILTERMODULE_H
#define FILTERMODULE_H

#include <QLineEdit>

#include "librarymodule.h"

namespace PhotoStage
{
class FilterModule : public LibraryModule
{
    Q_OBJECT

    public:

        FilterModule(QWidget* parent = 0);

    signals:

        void modelFilterApplied(const QString& filter);

    private slots:

        void onApplyFilter();

    private:

        QLineEdit* mLineEdit;
};
}
#endif // FILTERMODULE_H
