#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QPoint>
// modules
#include "module.h"
#include "library/library.h"
#include "develop/develop.h"
#include "map/map.h"
#include "databaseaccess.h"
#include "backgroundtaskmanager.h"
namespace Ui
{
    class MainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:

        explicit MainWindow(QWidget* parent = 0);
        ~MainWindow();

    private slots:

        // Modules
        void onModeLibraryClicked();
        void onModeDevelopClicked();
        void onModeMapClicked();

        // Dialogs
        void onActionImportTriggered();
        void onActionAboutTriggered();
        void onActionEditTimeTriggered();
        void onActionPreferences();

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
        void onSelectionChanged(const QItemSelection &, const QItemSelection &);

        // model changes
        void onModelReset();
        void onModelRowsInserted(const QModelIndex & parent, int start, int end);
        void onModelRowsRemoved(const QModelIndex & parent, int start, int end);

    private:

        Ui::MainWindow*        ui;
        BackgroundTaskManager* mBackgroundTaskManager;
        DatabaseAccess*        mDatabaseAccess;
        // module ui pointers
        Library*               mLibrary;
        Develop*               mDevelop;
        Map*                   mMap;
        Module*                mCurrentModule;
        PhotoModel*            mPhotoModel;
        PhotoWorkUnit*         mPhotoWorkUnit;
        QItemSelectionModel*   mPhotoSelection;

        //    QList<Photo*> mCurrentSelection;
        void setRating(int rating);
        void setColorLabel(Photo::ColorLabel color);
        void setFlag(Photo::Flag flag);
        void updateInformationBar();
};

#endif // MAINWINDOW_H
