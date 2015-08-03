#ifndef PHOTOSTAGE_LIBRARY_H
#define PHOTOSTAGE_LIBRARY_H

#include "module.h"

#include "photomodel.h"
#include "database/photodao.h"
#include "imagedbtile.h"
#include "modules/taggingmodule.h"
#include "modules/filtermodule.h"
#include "modules/filesmodule.h"
#include "modules/collectionmodule.h"
#include "modules/shortcutmodule.h"
#include "modules/libraryhistogrammodule.h"
#include "modules/metadatamodule.h"
#include "photosortfilterproxymodel.h"

namespace Ui
{
class Library;
}

namespace PhotoStage
{
class PhotoGridDndHandler;

class Library : public Module
{
    Q_OBJECT

    friend class PhotoGridDndHandler;

    public:

        explicit Library(PhotoSortFilterProxyModel* const model, QWidget* parent = 0);
        ~Library();

        QRect lightGap();

        void setSelectionModel(QItemSelectionModel* selectionModel);

        bool canSelectionChange();
        bool canSelectUpDown();
        int tilesPerRowOrCol();

        void reloadPathModel();

    signals:

        void photoSourceChanged(PhotoModel::SourceType type, long long id);
        void modelFilterApplied(const PhotoFilterInfo& info);

    public slots:

        // Library options bar
        void onShowLoupe();
        void onShowGrid();

        void onSortKeyChanged(int key);
        void onSortOrderChanged();

        void onPhotoSelectionChanged(const QItemSelection& selected, const QItemSelection&);
        void onCurrentPhotoChanged(const QModelIndex& current, const QModelIndex&);
        void onTileDoubleClicked(const QModelIndex&);

        void onCycleLoupeInfo();

        // For clicks on the tile
        //    void rotateLeftClicked(const QModelIndex& index);
        //    void rotateRightClicked(const QModelIndex& index);
        //    void ratingClicked(const QModelIndex& index, int rating);

    protected:

    private slots:

        void onNewCollectionClicked();
        void onCustomContextMenu(const QPoint& pos);
        void onThumbSizeChanged(int);
        void onZoomLevelChanged(int zoomLevel);
        void onPathSelected(long long pathid);

    private:

        Ui::Library*               ui;
        PhotoSortFilterProxyModel* mPhotoModel;
        PhotoDAO*                  mPhotoWorkUnit;
        TaggingModule*             mKeywording;
        LibraryHistogramModule*    mHistogramModule;
        MetaDataModule*            mMetaDataModule;
        QFont                      mFontAccessFoundIcons;
        Photo                      mCurrentPhoto;
        QItemSelectionModel*       mSelectionModel;
        FilesModule*               mFilesModule;
};
}

#endif // PHOTOSTAGE_LIBRARY_H
