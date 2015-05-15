#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void onActionImportTriggered();
    void onActionAboutTriggered();
    void onActionEditTimeTriggered();


private:
    Ui::MainWindow *ui;
    DatabaseAccess * mDatabaseAccess;

};

#endif // MAINWINDOW_H
