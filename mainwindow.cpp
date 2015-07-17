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

const QString SETTINGS_WINDOW_LOCATION          = "location";
const QString SETTINGS_SPLITTER_FILMSTRIP_SIZES = "splitter_filmstrip";

namespace PhotoStage
{
MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QApplication::setFont(QFont(QString("verdana"), 10));

    ui->setupUi(this);

    mDatabaseAccess = new DatabaseAccess();

    QSettings  settings;

    QList<int> l;
    settings.beginGroup("mainwindow");
    move(settings.value(SETTINGS_WINDOW_LOCATION).toPoint());

    if (settings.contains(SETTINGS_SPLITTER_FILMSTRIP_SIZES))
    {
        foreach(QVariant v, settings.value(
                SETTINGS_SPLITTER_FILMSTRIP_SIZES).toList())
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
    // setup connections
    connect(mPhotoModel, &PhotoModel::modelReset,
        this, &MainWindow::onModelReset);
    connect(mPhotoModel, &PhotoModel::rowsInserted,
        this, &MainWindow::onPhotoModelRowsInserted);
    connect(mPhotoModel, &PhotoModel::rowsRemoved,
        this, &MainWindow::onPhotoModelRowsRemoved);

    mPhotoSelection = new QItemSelectionModel(mPhotoModel, this);
    // set up connections
    connect(mPhotoSelection, &QItemSelectionModel::selectionChanged,
        this, &MainWindow::onSelectionChanged);
    connect(mPhotoSelection, &QItemSelectionModel::currentChanged,
        this, &MainWindow::onCurrentChanged);

    //**************
    // create the LIBRARY MODULE
    mLibrary = new Library(mPhotoModel, this);
    mLibrary->setSelectionModel(mPhotoSelection);
    ui->stackedWidget->addWidget(mLibrary);

    // setup connections
    connect(mLibrary, &Library::photoSourceChanged,
        mPhotoModel, &PhotoModel::onReloadPhotos);
    connect(ui->actionLoupeInfoCycle, &QAction::triggered,
        mLibrary, &Library::onCycleLoupeInfo);

    ui->filmStrip->setModel(mPhotoModel);
    FilmstripTile* fsTile = new FilmstripTile(ui->filmStrip);
    ui->filmStrip->setTileFlyweight(fsTile);
    ui->filmStrip->setMinimumCellHeight(80);
    ui->filmStrip->setTilesPerColRow(1);
    ui->filmStrip->setCheckBoxMode(false);
    ui->filmStrip->setOrientation(Qt::Horizontal);
    ui->filmStrip->setSelectionModel(mPhotoSelection);
    ui->filmStrip->setObjectName("Filmstrip");

    connect(ui->filmStrip, &TileView::TileView::doubleClickTile,
        this, &MainWindow::onTileDoubleClicked);

    //***************
    // Create the DEVELOP MODULE
    mDevelop = new Develop(this);
    ui->stackedWidget->addWidget(mDevelop);

    // ***************
    // Create the MAP MODULE
    mMap = new Map(mPhotoModel, this);
    ui->stackedWidget->addWidget(mMap);
    mMap->setSelectionModel(mPhotoSelection);

    // Set the current module variables
    mCurrentModule = mLibrary;
    ui->stackedWidget->setCurrentWidget(mLibrary);

    mPhotoWorkUnit = PhotoWorkUnit::instance();

    mBackgroundTaskManager = new BackgroundTaskManager(
        ui->scrollAreaWidgetContents, this);

    // Put all actions in groups.
    mActionStatePhoto.addAction(ui->actionColorBlue);
    mActionStatePhoto.addAction(ui->actionColorGreen);
    mActionStatePhoto.addAction(ui->actionColorNone);
    mActionStatePhoto.addAction(ui->actionColorOrange);
    mActionStatePhoto.addAction(ui->actionColorPurple);
    mActionStatePhoto.addAction(ui->actionColorRed);
    mActionStatePhoto.addAction(ui->actionColorYellow);

    mActionStatePhoto.addAction(ui->actionRating1);
    mActionStatePhoto.addAction(ui->actionRating2);
    mActionStatePhoto.addAction(ui->actionRating3);
    mActionStatePhoto.addAction(ui->actionRating4);
    mActionStatePhoto.addAction(ui->actionRating5);
    mActionStatePhoto.addAction(ui->actionRatingNone);
    mActionStatePhoto.addAction(ui->actionDecrease_Rating);
    mActionStatePhoto.addAction(ui->actionIncrease_rating);

    mActionStatePhoto.addAction(ui->actionFlagNone);
    mActionStatePhoto.addAction(ui->actionFlagPick);
    mActionStatePhoto.addAction(ui->actionFlagReject);

    mActionStatePhoto.addAction(ui->actionRotate_Left);
    mActionStatePhoto.addAction(ui->actionRotate_Right);
    mActionStatePhoto.addAction(ui->actionFlip_Horizontal);
    mActionStatePhoto.addAction(ui->actionFlip_Vertical);

    mActionStatePhoto.addAction(ui->actionShow_in_Finder);
    mActionStatePhoto.addAction(ui->actionDelete_Photo);
    mActionStatePhoto.addAction(ui->actionDelete_Rejected_Photos);

    mActionStatePhoto.addAction(ui->actionAdd_to_Quick_Collection);

    mActionStatePhoto.addAction(ui->actionEdit_Capture_Time);

    mActionStatePhoto.disableAll(true);

    qApp->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    //QDesktopWidget * desktop = QApplication::desktop();
    QSettings settings;

    settings.beginGroup("mainwindow");

    settings.setValue(SETTINGS_WINDOW_LOCATION, pos());
    QVariantList list;
    foreach(int size, ui->splitter->sizes())
    {
        list << size;
    }
    settings.setValue(SETTINGS_SPLITTER_FILMSTRIP_SIZES, list);
    delete ui;
    delete mDatabaseAccess;
}

bool MainWindow::eventFilter(QObject* /*obj*/, QEvent* event)
//bool MainWindow::event(QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* ke = static_cast<QKeyEvent*>(event);

        switch (ke->key())
        {
            case Qt::Key_Up:
                return selectUp();

            case Qt::Key_Down:
                return selectDown();

            case Qt::Key_Left:
                return selectPrevious();

            case Qt::Key_Right:
                return selectNext();

            case Qt::Key_Escape:
                mLibrary->showGrid();
                return true;

            case Qt::Key_Return:
            case Qt::Key_Enter:

                if (!mLibrary->canSelectionChange() &&
                    !ui->filmStrip->hasFocus())
                    return false;
                mLibrary->showLoupe();
                return true;
        }
    }
    return false;
    //    return QMainWindow::event(event);
}

bool MainWindow::selectNext()
{
    if (!mLibrary->canSelectionChange() && !ui->filmStrip->hasFocus())
        return false;

    QModelIndex index = mPhotoSelection->currentIndex();

    if (index.isValid())
    {
        int oldIndex = index.row();
        int newIndex = oldIndex;

        if (index.row()  < mPhotoModel->rowCount(QModelIndex()) - 1)
        {
            newIndex = oldIndex + 1;
            mPhotoSelection->setCurrentIndex(mPhotoModel->index(newIndex),
                QItemSelectionModel::ClearAndSelect);
            return true;
        }
    }
    return false;
}

bool MainWindow::selectPrevious()
{
    if (!mLibrary->canSelectionChange() && !ui->filmStrip->hasFocus())
        return false;

    QModelIndex index = mPhotoSelection->currentIndex();

    if (index.isValid())
    {
        int oldIndex = index.row();
        int newIndex = oldIndex;

        if (oldIndex > 0)
        {
            newIndex = oldIndex - 1;
            mPhotoSelection->setCurrentIndex(mPhotoModel->index(newIndex),
                QItemSelectionModel::ClearAndSelect);
            return true;
        }
    }
    return false;
}

bool MainWindow::selectUp()
{
    if (!mLibrary->canSelectionChange() && !mLibrary->canSelectUpDown())
        return false;

    QModelIndex index = mPhotoSelection->currentIndex();

    if (index.isValid())
    {
        int oldIndex = index.row();
        int newIndex = oldIndex;

        int tilesPerColRow = mLibrary->tilesPerRowOrCol();

        if (oldIndex >= tilesPerColRow)
        {
            newIndex = oldIndex - tilesPerColRow;

            mPhotoSelection->setCurrentIndex(mPhotoModel->index(newIndex),
                QItemSelectionModel::ClearAndSelect);
            return true;
        }
    }
    return false;
}

bool MainWindow::selectDown()
{
    if (!mLibrary->canSelectionChange() && !mLibrary->canSelectUpDown())
        return false;

    QModelIndex index = mPhotoSelection->currentIndex();

    if (index.isValid())
    {
        int oldIndex = index.row();
        int newIndex = oldIndex;

        int tilesPerColRow = mLibrary->tilesPerRowOrCol();

        if (oldIndex <
            mPhotoModel->rowCount(QModelIndex()) - tilesPerColRow)
        {
            newIndex = oldIndex + tilesPerColRow;

            mPhotoSelection->setCurrentIndex(mPhotoModel->index(newIndex),
                QItemSelectionModel::ClearAndSelect);
            return true;
        }
    }
    return false;
}

void MainWindow::onTileDoubleClicked(const QModelIndex&)
{
    mLibrary->showLoupe();
}

void MainWindow::onSelectionChanged(const QItemSelection& selected,
    const QItemSelection& /*deselected*/)
{
    //    QList<Photo> photos;
    //    foreach (QModelIndex index, selected.indexes())
    //    {
    //        photos.append(mPhotoModel->data(index,
    //            TileView::TileView::PhotoRole).value<Photo>());
    //    }
    updateInformationBar();
}

void MainWindow::onCurrentChanged(const QModelIndex& current,
    const QModelIndex& /*previous*/)
{
    setPhotoActionsAvailability(current.isValid());

    if (current.isValid())
    {
        mDevelop->setPhoto(currentPhoto());
    }

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

void MainWindow::onSelectAll()
{
    int            c           = mPhotoModel->rowCount(QModelIndex());
    QModelIndex    topLeft     = mPhotoModel->index(0, 0);
    QModelIndex    bottomRight = mPhotoModel->index(c - 1, 0);
    QItemSelection selection (topLeft, bottomRight);

    mPhotoSelection->select(selection, QItemSelectionModel::Select);
}

void MainWindow::onSelectNone()
{
    mPhotoSelection->clear();
}

void MainWindow::onActionImportTriggered()
{
    ImportDialog* importDialog = new ImportDialog(this);
    int           resultCode   = importDialog->exec();

    if (resultCode == QDialog::Accepted)
    {
        ImportBackgroundTask* r = new ImportBackgroundTask(
            importDialog->importInfo());
        mBackgroundTaskManager->addRunnable(r);
        r->start();
        connect(r, &ImportBackgroundTask::taskFinished,
            this, &MainWindow::importFinished);
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
        TranslucentWindow* w    = new TranslucentWindow();
        QRect              rect = d->screenGeometry(i);
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

    task->deleteLater();
    // update the files tree as well and the collection tree

    mLibrary->onPathModelRowsAdded(QModelIndex(), 0, 0);
}

void MainWindow::onModelReset()
{
    setPhotoActionsAvailability(false);
    updateInformationBar();
}

void MainWindow::onPhotoModelRowsInserted(const QModelIndex& /*parent*/,
    int /*start*/,
    int /*end*/)
{
    updateInformationBar();
}

void MainWindow::onPhotoModelRowsRemoved(const QModelIndex& /*parent*/,
    int /*start*/,
    int /*end*/)
{
    updateInformationBar();
}

void MainWindow::onShowGrid()
{
    // force library module to the front
    onModeLibraryClicked();
    mLibrary->showGrid();
}

void MainWindow::onShowLoupe()
{
    // force library module to the front
    onModeLibraryClicked();
    mLibrary->showLoupe();
}

void MainWindow::updateInformationBar()
{
    QString info;
    int     count    = mPhotoModel->rowCount(QModelIndex());
    int     selCount = mPhotoSelection->selectedIndexes().size();

    QString imagePath;
    Photo   photo = currentPhoto();

    if (!photo.isNull())
        imagePath = " " + photo.srcImagePath();
    ui->lblInformation->setText(QString::number(selCount) +
        "/" + QString::number(count) + imagePath);
}

Photo MainWindow::currentPhoto()
{
    return mPhotoModel->data(mPhotoSelection->currentIndex(),
               TileView::TileView::PhotoRole).value<Photo>();
}

void MainWindow::setRating(int rating)
{
    QList<Photo>    list;
    QModelIndexList indexes = mPhotoSelection->selectedIndexes();
    foreach (QModelIndex index, indexes)
    list.append(mPhotoModel->data(index,
        TileView::TileView::PhotoRole).value<Photo>());
    mPhotoWorkUnit->setRating(list, rating);
    QVector<int> roles;
    roles.append(TileView::TileView::PhotoRole);
    mPhotoModel->refreshData(list);
}

void MainWindow::setFlag(Photo::Flag flag)
{
    QList<Photo>    list;
    QModelIndexList indexes = mPhotoSelection->selectedIndexes();
    foreach (QModelIndex index, indexes)
    list.append(mPhotoModel->data(index,
        TileView::TileView::PhotoRole).value<Photo>());

    mPhotoWorkUnit->setFlag(list, flag);
    QVector<int> roles;
    roles.append(TileView::TileView::PhotoRole);
    mPhotoModel->refreshData(list);
}

void MainWindow::setColorLabel(Photo::ColorLabel color)
{
    QList<Photo>    list;
    QModelIndexList indexes = mPhotoSelection->selectedIndexes();
    foreach (QModelIndex index, indexes)
    list.append(mPhotoModel->data(index,
        TileView::TileView::PhotoRole).value<Photo>());

    mPhotoWorkUnit->setColorLabel(list, color);
    QVector<int> roles;
    roles.append(TileView::TileView::PhotoRole);
    mPhotoModel->refreshData(list);
}

void MainWindow::setPhotoActionsAvailability(bool enabled)
{
    mActionStatePhoto.enableAll(enabled);
}
}
