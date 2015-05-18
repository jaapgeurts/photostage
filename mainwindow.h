#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>

#include "databaseaccess.h"
#include "sqlphotomodel.h"
#include "photoworkunit.h"
#include "librarymodules/keywordingmodule.h"

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
    void onActionImportTriggered();
    void onActionAboutTriggered();
    void onActionEditTimeTriggered();

    void customContextMenu(const QPoint& pos);

    // For clicks on the tile
    void rotateLeftClicked(const QModelIndex& index);
    void rotateRightClicked(const QModelIndex& index);
    void ratingClicked(const QModelIndex& index, int rating);

    // called when the selection in the photo tile view changes
    void selectionChanged();

private:
    Ui::MainWindow *ui;
    DatabaseAccess * mDatabaseAccess;
    SqlPhotoModel * mPhotoModel;
    PhotoWorkUnit * mPhotoWorkUnit;
    KeywordingModule *mKeywording;

};

#endif // MAINWINDOW_H
