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
#include "modules/metadatamodule.h"
#include "widgets/fixedtreeview.h"

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

        bool canSelectionChange();
        bool canSelectUpDown();
        int tilesPerRowOrCol();

    signals:

        void photoSourceChanged(PhotoModel::SourceType type, long long id);

    public slots:

        void showLoupe();
        void showGrid();

        void onPhotoSelectionChanged(const QItemSelection& selected,
            const QItemSelection&);
        void onCurrentPhotoChanged(const QModelIndex& current, const QModelIndex&);
        void onPathModelRowsAdded(const QModelIndex&, int, int);
        void onPathModelRowsRemoved(const QModelIndex&, int, int);
        void onTileDoubleClicked(const QModelIndex&);

        // For clicks on the tile
        //    void rotateLeftClicked(const QModelIndex& index);
        //    void rotateRightClicked(const QModelIndex& index);
        //    void ratingClicked(const QModelIndex& index, int rating);

    protected:

    private slots:

        void onNewCollectionClicked();
        void onFilesClicked(const QModelIndex&);
        void customContextMenu(const QPoint&);
        void thumbSizeChanged(int);

    private:

        Ui::Library*            ui;
        PhotoModel*             mPhotoModel;
        PhotoWorkUnit*          mPhotoWorkUnit;
        SqlPathModel*           mPathModel;
        TaggingModule*          mKeywording;
        LibraryHistogramModule* mHistogramModule;
        MetaDataModule*         mMetaDataModule;
        QFont                   mFontAccessFoundIcons;
        Photo                   mCurrentPhoto;
        FixedTreeView*          mTrvwFiles;
};
}

#endif // LIBRARY_H
