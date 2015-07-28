#ifndef PHOTOSTAGE_LIBRARY_H
#define PHOTOSTAGE_LIBRARY_H

#include "module.h"

#include "photomodel.h"
#include "sqlpathmodel.h"
#include "database/photoworkunit.h"
#include "imagedbtile.h"
#include "modules/taggingmodule.h"
#include "modules/filtermodule.h"
#include "modules/collectionmodule.h"
#include "modules/shortcutmodule.h"
#include "modules/libraryhistogrammodule.h"
#include "modules/metadatamodule.h"
#include "widgets/fixedtreeview.h"
#include "photosortfilterproxymodel.h"

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

        explicit Library(PhotoSortFilterProxyModel* const model,
            QWidget* parent = 0);
        ~Library();

        QRect lightGap();

        void setSelectionModel(QItemSelectionModel* selectionModel);

        bool canSelectionChange();
        bool canSelectUpDown();
        int tilesPerRowOrCol();

    signals:

        void photoSourceChanged(PhotoModel::SourceType type, long long id);
        void modelFilterApplied(const PhotoFilterInfo& info);

    public slots:

        // Library options bar
        void onShowLoupe();
        void onShowGrid();

        void onSortKeyChanged(int key);
        void onSortOrderChanged();

        void onPhotoSelectionChanged(const QItemSelection& selected,
            const QItemSelection&);
        void onCurrentPhotoChanged(const QModelIndex& current,
            const QModelIndex&);
        void onPathModelRowsAdded(const QModelIndex&, int, int);
        void onPathModelRowsRemoved(const QModelIndex&, int, int);
        void onTileDoubleClicked(const QModelIndex&);

        void onCycleLoupeInfo();

        // For clicks on the tile
        //    void rotateLeftClicked(const QModelIndex& index);
        //    void rotateRightClicked(const QModelIndex& index);
        //    void ratingClicked(const QModelIndex& index, int rating);

    protected:

    private slots:

        void onNewCollectionClicked();
        void onFilesClicked(const QModelIndex&);
        void customContextMenu(const QPoint&);
        void onThumbSizeChanged(int);
        void onZoomLevelChanged(int zoomLevel);

    private:

        Ui::Library*               ui;
        PhotoSortFilterProxyModel* mPhotoModel;
        PhotoWorkUnit*             mPhotoWorkUnit;
        SqlPathModel*              mPathModel;
        TaggingModule*             mKeywording;
        LibraryHistogramModule*    mHistogramModule;
        MetaDataModule*            mMetaDataModule;
        QFont                      mFontAccessFoundIcons;
        Photo                      mCurrentPhoto;
        FixedTreeView*             mTrvwFiles;
};
}

#endif // PHOTOSTAGE_LIBRARY_H
