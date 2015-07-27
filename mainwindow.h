#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include <QPoint>

// modules
#include "module.h"
#include "library/library.h"
#include "develop/develop.h"
#include "widgets/actionstategroup.h"
#include "cartography/map.h"
#include "databaseaccess.h"
#include "backgroundtaskmanager.h"
#include "photosortfilterproxymodel.h"

namespace Ui
{
class MainWindow;
}

namespace PhotoStage
{
class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:

        explicit MainWindow(QWidget* parent = 0);
        ~MainWindow();

    public slots:

        bool selectNext();
        bool selectPrevious();
        bool selectUp();
        bool selectDown();

    protected:

        bool eventFilter(QObject*, QEvent* event);
        void closeEvent(QCloseEvent* event);
        //bool event(QEvent* event);

    private slots:

        // Modules
        void onModeLibraryClicked();
        void onModeDevelopClicked();
        void onModeMapClicked();

        //Edit menu
        void onSelectAll();
        void onSelectNone();

        //Library menu
        void onActionRegenHashes();

        // Dialogs
        void onActionImportTriggered();
        void onActionAboutTriggered();
        void onActionEditTimeTriggered();
        void onActionPreferences();

        // Photo
        // Rating
        void onActionRating1();
        void onActionRating2();
        void onActionRating3();
        void onActionRating4();
        void onActionRating5();
        void onActionRatingNone();

        // flags
        void onActionFlagPick();
        void onActionFlagReject();
        void onActionFlagNone();

        // color labels
        void onActionColorNone();
        void onActionColorRed();
        void onActionColorGreen();
        void onActionColorBlue();
        void onActionColorYellow();
        void onActionColorOrange();
        void onActionColorPurple();
        void onActionLightsOff();

        // called in response to the import thread
        void importFinished(BackgroundTask* task);
        void onSelectionChanged(const QItemSelection& selected,
            const QItemSelection&);
        void onCurrentChanged(const QModelIndex& current, const QModelIndex&);

        // model changes
        void onModelReset();
        void onPhotoModelRowsInserted(const QModelIndex& parent,
            int start, int end);
        void onPhotoModelRowsRemoved(const QModelIndex& parent,
            int start, int end);
        void onFilterApplied(const PhotoFilterInfo& info);
        void onPhotoSourceChanged(PhotoModel::SourceType type, long long id);

        void onShowGrid();
        void onShowLoupe();

        void onTileDoubleClicked(const QModelIndex&);

    private:

        Ui::MainWindow*            ui;
        BackgroundTaskManager*     mBackgroundTaskManager;
        DatabaseAccess*            mDatabaseAccess;
        // module ui pointers
        Library*                   mLibrary;
        Develop*                   mDevelop;
        Map*                       mMap;
        Module*                    mCurrentModule;
        PhotoSortFilterProxyModel* mPhotoModelProxy;
        PhotoModel*                mSourceModel;
        PhotoWorkUnit*             mPhotoWorkUnit;
        QItemSelectionModel*       mPhotoSelection;
        ActionStateGroup           mActionStatePhoto;

        //    QList<Photo> mCurrentSelection;
        void setRating(int rating);
        void setColorLabel(Photo::ColorLabel color);
        void setFlag(Photo::Flag flag);
        void updateInformationBar();

        Photo currentPhoto();
        void setPhotoActionsAvailability(bool enabled);
};
}

#endif // MAINWINDOW_H
