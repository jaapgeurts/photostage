#include <QApplication>
#include <QTreeView>
#include <QTextEdit>
#include <QLineEdit>
#include <QDesktopWidget>
#include <QVariantList>

#include "mainwindow.h"
#include "ui_mainwindow.h"

// Models
#include "photomodel.h"
#include "photo.h"

// dialogs
#include "aboutdialog.h"
#include "timeadjustdialog.h"
#include "import/importdialog.h"
#include "preferencesdialog.h"

#include "widgets/translucentwindow.h"
#include "widgets/backgroundtaskprogress.h"

#include "backgroundtask.h"
#include "import/importbackgroundtask.h"
#include "filmstriptile.h"

#define SETTINGS_WINDOW_LOCATION "mainwindow/location"
#define SETTINGS_SPLITTER_FILMSTRIP_SIZES "mainwindow/splitter_filmstrip"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    QApplication::setFont(QFont(QString("verdana"),10));

    ui->setupUi(this);

    setWindowIcon(QIcon(":/appicon.png"));

    mDatabaseAccess = new DatabaseAccess();

    QSettings settings;
    move(settings.value(SETTINGS_WINDOW_LOCATION).toPoint());

    QList<int> l;

    if (settings.contains(SETTINGS_SPLITTER_FILMSTRIP_SIZES))
    {
        foreach(QVariant v, settings.value(SETTINGS_SPLITTER_FILMSTRIP_SIZES).toList())
        {
            l << v.toInt();
        }
    }
    else
    {
        l << 600 << 200;
    }
    ui->splitter->setSizes(l);

    mPhotoModel = new PhotoModel(this);
    connect(mPhotoModel,&PhotoModel::modelReset,this,&MainWindow::onModelReset);
    connect(mPhotoModel,&PhotoModel::rowsInserted,this,&MainWindow::onModelRowsInserted);
    connect(mPhotoModel,&PhotoModel::rowsRemoved,this,&MainWindow::onModelRowsRemoved);

    mPhotoSelection = new QItemSelectionModel(mPhotoModel,this);
    connect(mPhotoSelection,&QItemSelectionModel::selectionChanged, this, &MainWindow::onSelectionChanged);

    // Create the Library Module
    mLibrary = new Library(mPhotoModel,this);
    mLibrary->setSelectionModel(mPhotoSelection);
    connect(mLibrary,&Library::photoSourceChanged, mPhotoModel, &PhotoModel::onReloadPhotos);
    ui->stackedWidget->addWidget(mLibrary);

    ui->filmStrip->setModel(mPhotoModel);
    FilmstripTile* fsTile = new FilmstripTile(ui->filmStrip);
    ui->filmStrip->setTileFlyweight(fsTile);
    ui->filmStrip->setMinimumCellHeight(80);
    ui->filmStrip->setMaxRows(1);
    ui->filmStrip->setCheckBoxMode(false);
    ui->filmStrip->setOrientation(Qt::Horizontal);
    ui->filmStrip->setSelectionModel(mPhotoSelection);

    // Create the Develop Module
    mDevelop = new Develop(this);
    ui->stackedWidget->addWidget(mDevelop);

    mMap = new Map(this);
    ui->stackedWidget->addWidget(mMap);

    // Set the current module variables
    mCurrentModule = mLibrary;
    ui->stackedWidget->setCurrentWidget(mLibrary);

    mPhotoWorkUnit = PhotoWorkUnit::instance();

    mBackgroundTaskManager = new BackgroundTaskManager(ui->scrollAreaWidgetContents, this);
}

MainWindow::~MainWindow()
{
    //QDesktopWidget * desktop = QApplication::desktop();
    QSettings settings;
    settings.setValue(SETTINGS_WINDOW_LOCATION,pos());
    QVariantList list;
    foreach(int size, ui->splitter->sizes())
    {
        list << size;
    }
    settings.setValue(SETTINGS_SPLITTER_FILMSTRIP_SIZES,list);
    delete ui;
    delete mDatabaseAccess;
}

void MainWindow::onSelectionChanged(const QItemSelection & /*selected*/, const QItemSelection & /*deselected*/)
{
//    mCurrentSelection = list;
    updateInformationBar();
}

void MainWindow::onModeLibraryClicked()
{
    ui->stackedWidget->setCurrentWidget(mLibrary);
}

void MainWindow::onModeDevelopClicked()
{
    ui->stackedWidget->setCurrentWidget(mDevelop);
}

void MainWindow::onModeMapClicked()
{
    ui->stackedWidget->setCurrentWidget(mMap);
}

void MainWindow::onActionImportTriggered()
{

    ImportDialog* importDialog = new ImportDialog(this);
    int resultCode = importDialog->exec();

    if (resultCode == QDialog::Accepted)
    {
        ImportBackgroundTask* r = new ImportBackgroundTask(importDialog->importInfo());
        mBackgroundTaskManager->addRunnable(r);
        r->start();
        connect(r,&ImportBackgroundTask::taskFinished,this,&MainWindow::importFinished);
    }
    delete importDialog;
}

void MainWindow::onActionAboutTriggered()
{
    AboutDialog* aboutDialog = new AboutDialog(this);
    /*int code = */ aboutDialog->exec();
    delete aboutDialog;
}

void MainWindow::onActionEditTimeTriggered()
{
    TimeAdjustDialog* timeAdjustDialog = new TimeAdjustDialog(this);
    /*int code = */ timeAdjustDialog->exec();
    delete timeAdjustDialog;
}

void MainWindow::onActionPreferences()
{
    PreferencesDialog prefs(this);
    prefs.exec();
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

void MainWindow::onActionFlagPick()
{
    setFlag(Photo::FlagPick);
}

void MainWindow::onActionFlagReject()
{
    setFlag(Photo::FlagReject);
}

void MainWindow::onActionFlagNone()
{
    setFlag(Photo::FlagNone);
}

void MainWindow::onActionColorNone()
{
    setColorLabel(Photo::LabelNoColor);
}

void MainWindow::onActionColorRed()
{
    setColorLabel(Photo::LabelRed);
}

void MainWindow::onActionColorGreen()
{
    setColorLabel(Photo::LabelGreen);
}

void MainWindow::onActionColorBlue()
{
    setColorLabel(Photo::LabelBlue);
}

void MainWindow::onActionColorYellow()
{
    setColorLabel(Photo::LabelYellow);
}

void MainWindow::onActionColorOrange()
{
    setColorLabel(Photo::LabelOrange);
}

void MainWindow::onActionColorPurple()
{
    setColorLabel(Photo::LabelPurple);
}

void MainWindow::onActionLightsOff()
{

    //w->showFullScreen();
    QDesktopWidget* d = QApplication::desktop();

    for (int i = 0; i < d->screenCount(); i++)
    {
        qDebug() << "Lights off on screen:" << i;
        TranslucentWindow* w = new TranslucentWindow();
        QRect rect = d->screenGeometry(i);
        qDebug() << "Window" << i << "size:" << rect;
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

void MainWindow::importFinished(BackgroundTask* task)
{
    ImportBackgroundTask* t = static_cast<ImportBackgroundTask*>(task);
    mPhotoModel->addData(t->resultList());

    // update the files tree as well and the collection tree

    task->deleteLater();
}

void MainWindow::onModelReset()
{
    updateInformationBar();
}

void MainWindow::onModelRowsInserted(const QModelIndex & /*parent*/, int /*start*/, int /*end*/)
{
    updateInformationBar();
}

void MainWindow::onModelRowsRemoved(const QModelIndex & /*parent*/, int /*start*/, int /*end*/)
{
    updateInformationBar();
}

void MainWindow::updateInformationBar()
{
    QString info;
    int count = mPhotoModel->rowCount(QModelIndex());
    int selCount = mPhotoSelection->selectedIndexes().size();
    ui->lblInformation->setText(QString::number(selCount) + "/" + QString::number(count));
}

void MainWindow::setRating(int rating)
{
    QList<Photo*> list;
    QModelIndexList indexes = mPhotoSelection->selectedIndexes();
    foreach (QModelIndex index, indexes)
    list.append(mPhotoModel->data(index,TileView::PhotoRole).value<Photo*>());
    mPhotoWorkUnit->setRating(list,rating);
    QVector<int> roles;
    roles.append(TileView::PhotoRole);
    mPhotoModel->refreshData(list);
}

void MainWindow::setFlag(Photo::Flag flag)
{
    QList<Photo*> list;
    QModelIndexList indexes = mPhotoSelection->selectedIndexes();
    foreach (QModelIndex index, indexes)
    list.append(mPhotoModel->data(index,TileView::PhotoRole).value<Photo*>());

    mPhotoWorkUnit->setFlag(list,flag);
    QVector<int> roles;
    roles.append(TileView::PhotoRole);
    mPhotoModel->refreshData(list);
}

void MainWindow::setColorLabel(Photo::ColorLabel color)
{
    QList<Photo*> list;
    QModelIndexList indexes = mPhotoSelection->selectedIndexes();
    foreach (QModelIndex index, indexes)
    list.append(mPhotoModel->data(index,TileView::PhotoRole).value<Photo*>());

    mPhotoWorkUnit->setColorLabel(list,color);
    QVector<int> roles;
    roles.append(TileView::PhotoRole);
    mPhotoModel->refreshData(list);
}
