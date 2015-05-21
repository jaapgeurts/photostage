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


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void selectionChanged(const QList<SqlPhotoInfo> & list);

    // Modules
    void onModeLibraryClicked();
    void onModeDevelopClicked();
    void onModeMapClicked();

    // Menu actions
    void onActionImportTriggered();
    void onActionAboutTriggered();

    void onActionEditTimeTriggered();

    // Rating
    void onActionRating1();
    void onActionRating2();
    void onActionRating3();
    void onActionRating4();
    void onActionRating5();
    void onActionRatingNone();

    void onActionLightsOff();


    // called when the selection in the photo tile view changes

private:
    Ui::MainWindow *ui;
    DatabaseAccess * mDatabaseAccess;
    // module ui pointers
    Library* mLibrary;
    Develop* mDevelop;
    Map* mMap;
    Module* mCurrentModule;

    SqlPhotoModel * mPhotoModel;
    PhotoWorkUnit * mPhotoWorkUnit;

    QList<SqlPhotoInfo> mCurrentSelection;

    void setRating(int rating);

};

#endif // MAINWINDOW_H
