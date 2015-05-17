#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>

#include "databaseaccess.h"
#include "sqlphotomodel.h"

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

private:
    Ui::MainWindow *ui;
    DatabaseAccess * mDatabaseAccess;
    SqlPhotoModel * mPhotoModel;

};

#endif // MAINWINDOW_H
