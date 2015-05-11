#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "importdialog.h"

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
//    void onDialogImportFinished(int resultCode);

private:
    Ui::MainWindow *ui;
//    ImportDialog* mImportDialog;
};

#endif // MAINWINDOW_H
