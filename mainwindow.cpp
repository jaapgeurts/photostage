#include <QStringList>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "importworkunit.h"


using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}


MainWindow::~MainWindow()
{
    delete ui;
}

//void MainWindow::onDialogImportFinished(int resultCode)
//{
//    mImportDialog->deleteLater();
//    mImportDialog = NULL;
//}

void MainWindow::onActionImportTriggered()
{
    ImportDialog * importDialog = new ImportDialog(this);
    //mImportDialog->connect(mImportDialog,SIGNAL(finished(int)),this,SLOT(onDialogImportFinished(int)));
    ImportWorkUnit *workUnit = new ImportWorkUnit();
    int resultCode = importDialog->exec();
    if (resultCode == QDialog::Accepted)
    {
        workUnit->importPhotos(importDialog->getImportInfo());
        delete workUnit;
    }
    delete importDialog;
}
