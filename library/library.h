#ifndef LIBRARY_H
#define LIBRARY_H

#include "module.h"

#include "photomodel.h"
#include "sqlpathmodel.h"
#include "workunits/photoworkunit.h"
#include "imagedbtile.h"
#include "modules/taggingmodule.h"
#include "modules/collectionmodule.h"
#include "modules/shortcutmodule.h"
#include "modules/libraryhistogrammodule.h"

namespace Ui
{
    class Library;
}

namespace PhotoStage
{
    class Library : public Module
    {
        Q_OBJECT

        public:

            explicit Library(PhotoModel* const model, QWidget* parent = 0);
            ~Library();

            QRect lightGap();

            void setSelectionModel(QItemSelectionModel* selectionModel);

        signals:

            void photoSelectionChanged(const QList<Photo*>& list);
            void photoSourceChanged(PhotoModel::SourceType type, long long id);

        public slots:

            void onPhotoSelectionChanged(const QItemSelection& selected,
                const QItemSelection&);
            void onTileDoubleClicked(const QModelIndex& index);

            // For clicks on the tile
            //    void rotateLeftClicked(const QModelIndex& index);
            //    void rotateRightClicked(const QModelIndex& index);
            //    void ratingClicked(const QModelIndex& index, int rating);

        protected:

            bool event(QEvent* event);

        private slots:

            void onNewCollectionClicked();
            void onFilesClicked(const QModelIndex&);
            void customContextMenu(const QPoint& pos);

        private:

            Ui::Library*            ui;
            PhotoModel*             mPhotoModel;
            PhotoWorkUnit*          mPhotoWorkUnit;
            SqlPathModel*           mPathModel;
            TaggingModule*          mKeywording;
            LibraryHistogramModule* mHistogramModule;
    };
}

#endif // LIBRARY_H