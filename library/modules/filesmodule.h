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

        void reload();

    signals:

        void pathSelected(long long pathid);

    protected:

        bool eventFilter(QObject* obj, QEvent* event);

    private:

        FixedTreeView* mTrvwFiles;
        PathModel*  mPathModel;

        void onPathModelRowsAdded(const QModelIndex&, int, int);
        void onPathModelRowsRemoved(const QModelIndex&, int, int);
        void onFilesClicked(const QModelIndex&);
        bool handleDrop(QDropEvent* event);
};
}
#endif // PHOTOSTAGE_FILESMODULE_H
