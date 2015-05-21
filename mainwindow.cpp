#include <QStringList>
#include <QSqlTableModel>
#include <QTreeView>
#include <QTextEdit>
#include <QLineEdit>
#include <QApplication>
#include <QDesktopWidget>


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "workunits/importworkunit.h"

// Models
#include "sqlphotomodel.h"
#include "sqlphotoinfo.h"

// dialogs
#include "aboutdialog.h"
#include "timeadjustdialog.h"
#include "import/importdialog.h"

#include "widgets/translucentwindow.h"




using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    QApplication::setFont(QFont(QString("verdana"),10));

    ui->setupUi(this);

    setWindowIcon(QIcon(":/appicon.png"));

    mDatabaseAccess = new DatabaseAccess();

    QSettings settings;
    move(settings.value("mainwindow/location").toPoint());

    mPhotoModel = new SqlPhotoModel(this);

    mLibrary = new Library(mPhotoModel,this);
    connect(mLibrary,(void (Library::*)(const QList<SqlPhotoInfo>&))&Library::selectionChanged, this, &MainWindow::selectionChanged);
    int index = ui->stackedWidget->addWidget(mLibrary);
    ui->stackedWidget->setCurrentIndex(index);
    mCurrentModule = mLibrary;

    mPhotoWorkUnit = PhotoWorkUnit::instance();

}

MainWindow::~MainWindow()
{
    //QDesktopWidget * desktop = QApplication::desktop();
    QSettings settings;
    settings.setValue("mainwindow/location",pos());
    delete ui;
    delete mDatabaseAccess;
}

void MainWindow::selectionChanged(const QList<SqlPhotoInfo> &list)
{
    mCurrentSelection = list;
}

void MainWindow::onActionImportTriggered()
{
    ImportDialog * importDialog = new ImportDialog(this);
    ImportWorkUnit *workUnit = new ImportWorkUnit();
    int resultCode = importDialog->exec();
    if (resultCode == QDialog::Accepted)
    {
        workUnit->importPhotos(importDialog->getImportInfo());
        delete workUnit;
    }
    delete importDialog;
}

void MainWindow::onActionAboutTriggered()
{
    AboutDialog * aboutDialog = new AboutDialog(this);
    /*int code = */aboutDialog->exec();
    delete aboutDialog;
}


void MainWindow::onActionEditTimeTriggered()
{
    TimeAdjustDialog* timeAdjustDialog = new TimeAdjustDialog(this);
    /*int code = */timeAdjustDialog->exec();
    delete timeAdjustDialog;
}

void MainWindow::onActionRating1()
{
    setRating(1);
}

void MainWindow::onActionRating2()
{
    setRating(2);
}

void MainWindow::onActionRating3()
{
    setRating(3);
}

void MainWindow::onActionRating4()
{
    setRating(4);
}

void MainWindow::onActionRating5()
{
    setRating(5);
}

void MainWindow::onActionRatingNone()
{
    setRating(0);
}

void MainWindow::onActionLightsOff()
{

    //w->showFullScreen();
    QDesktopWidget * d = QApplication::desktop();
    for(int i =0; i < d->screenCount(); i++)
    {
        qDebug() << "Lights off on screen:" << i;
        TranslucentWindow* w = new TranslucentWindow();
        QRect rect = d->screenGeometry(i);
        qDebug() << "Window"<<i<<"size:"<<rect;
        w->move(rect.topLeft());
        w->resize(rect.size());
        // ask the module for the rectangle
        QRect gap = mCurrentModule->lightGap();
        w->setGapGeometry(gap);
        w->show();

        //w->showMaximized();
        //w->showFullScreen();
    }
}

void MainWindow::setRating(int rating)
{
    mPhotoWorkUnit->setRating(mCurrentSelection,rating);
    QVector<int> roles;
    roles.append(TileView::PhotoRole);
    mPhotoModel->updateData(mCurrentSelection);
}





