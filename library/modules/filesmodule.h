#ifndef PHOTOSTAGE_FILESMODULE_H
#define PHOTOSTAGE_FILESMODULE_H

#include <QWidget>

#include "pathmodel.h"
#include "librarymodule.h"
#include "widgets/fixedtreeview.h"

namespace PhotoStage
{
class FilesModule : public LibraryModule
{
    Q_OBJECT

    public:

        FilesModule(QWidget* parent = 0);
        ~FilesModule();

    signals:

        void pathSelected(long long pathid);

    private slots:

        void onFilesClicked(const QModelIndex& index);

    private:

        Widgets::FixedTreeView* mTrvwFiles;
        PathModel*              mPathModel;

        bool handleDrop(QDropEvent* event);
};
}
#endif // PHOTOSTAGE_FILESMODULE_H
