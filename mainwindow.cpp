#include <QApplication>
#include <QMessageBox>
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

// widget related stuf
#include "widgets/translucentwindow.h"
#include "widgets/backgroundtaskprogress.h"
#include "filmstriptile.h"

#include "backgroundtask.h"
#include "regenhashestask.h"
#include "import/importbackgroundtask.h"

#include "constants.h"

#include "external/xxHash/xxhash.h"

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

    mSourceModel     = new PhotoModel(this);
    mPhotoModelProxy = new PhotoSortFilterProxyModel(this);
    mPhotoModelProxy->setSourceModel(mSourceModel);

    // setup PhotoModel connections
    connect(mSourceModel, &PhotoModel::modelReset, this, &MainWindow::onModelReset);
    connect(mSourceModel, &PhotoModel::rowsInserted, this, &MainWindow::onPhotoModelRowsInserted);
    connect(mSourceModel, &PhotoModel::rowsRemoved, this, &MainWindow::onPhotoModelRowsRemoved);

    mPhotoSelection = new QItemSelectionModel(mPhotoModelProxy, this);
    // set up connections
    connect(mPhotoSelection, &QItemSelectionModel::selectionChanged, this, &MainWindow::onSelectionChanged);
    connect(mPhotoSelection, &QItemSelectionModel::currentChanged, this, &MainWindow::onCurrentChanged);

    //**************
    // create the LIBRARY MODULE
    mLibrary = new Library(mPhotoModelProxy, this);
    mLibrary->setSelectionModel(mPhotoSelection);
    ui->stackedWidget->addWidget(mLibrary);

    // setup connections
    connect(mLibrary, &Library::photoSourceChanged, this, &MainWindow::onPhotoSourceChanged);
    connect(mLibrary, &Library::modelFilterApplied, this, &MainWindow::onFilterApplied);
    connect(mLibrary, &Library::customContextMenuRequested, this, &MainWindow::onLibraryContextMenu);

    connect(ui->actionLoupeInfoCycle, &QAction::triggered, mLibrary, &Library::onCycleLoupeInfo);

    ui->filmStrip->setModel(mPhotoModelProxy);
    ui->filmStrip->setDragEnabled(true);
    ui->filmStrip->setAcceptDrops(false);
    FilmstripTile* fsTile = new FilmstripTile(ui->filmStrip);
    ui->filmStrip->setTileFlyweight(fsTile);
    ui->filmStrip->setMinimumCellHeight(80);
    ui->filmStrip->setTilesPerColRow(1);
    ui->filmStrip->setCheckBoxMode(false);
    ui->filmStrip->setOrientation(Qt::Horizontal);
    ui->filmStrip->setSelectionModel(mPhotoSelection);
    ui->filmStrip->setObjectName("Filmstrip");

    connect(ui->filmStrip, &Widgets::TileView::doubleClickTile, this, &MainWindow::onTileDoubleClicked);
    connect(ui->filmStrip, &Widgets::TileView::visibleTilesChanged, mSourceModel, &PhotoModel::onVisibleTilesChanged);

    //***************
    // Create the DEVELOP MODULE
    mDevelop = new Develop(this);
    ui->stackedWidget->addWidget(mDevelop);

    // ***************
    // Create the MAP MODULE
    mMap = new Map(mPhotoModelProxy, this);
    ui->stackedWidget->addWidget(mMap);
    mMap->setSelectionModel(mPhotoSelection);

    // Set the current module variables
    mCurrentModule = mLibrary;
    ui->stackedWidget->setCurrentWidget(mLibrary);

    mPhotoWorkUnit = PhotoDAO::instance();

    mBackgroundTaskManager = new BackgroundTaskManager(ui->scrollAreaWidgetContents, this);

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

    mActionStatePhoto.addAction(ui->actionShowInFileBrowser);
    mActionStatePhoto.addAction(ui->actionDeletePhotos);
    mActionStatePhoto.addAction(ui->actionDeleteRejectedPhotos);

    mActionStatePhoto.addAction(ui->actionAdd_to_Quick_Collection);

    mActionStatePhoto.addAction(ui->actionEdit_Capture_Time);

    mActionStatePhoto.disableAll(true);

    qApp->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    mBackgroundTaskManager->cancelAll();
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
                mLibrary->onShowGrid();
                return true;

            case Qt::Key_Return:
            case Qt::Key_Enter:

                if (!mLibrary->canSelectionChange() &&
                    !ui->filmStrip->hasFocus())
                    return false;
                mLibrary->onShowLoupe();
                return true;
        }
    }
    return false;
    //    return QMainWindow::event(event);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (mBackgroundTaskManager->isWorking())
    {
        int dlgResult = QMessageBox::warning(this,
                "Running active tasks",
                "There are active tasks running. Do you wish to stop them and quit?",
                "Stop tasks and quit",
                "Keep running",
                QString(),
                1);

        if (dlgResult == 1)
            event->ignore();
        else
            event->accept(); // close
    }
    else
        event->accept(); // close
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

        if (index.row()  < mPhotoModelProxy->rowCount(QModelIndex()) - 1)
        {
            newIndex = oldIndex + 1;
            mPhotoSelection->setCurrentIndex(mPhotoModelProxy->index(newIndex,
                0),
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
            mPhotoSelection->setCurrentIndex(mPhotoModelProxy->index(newIndex,
                0),
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

            mPhotoSelection->setCurrentIndex(mPhotoModelProxy->index(newIndex,
                0),
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
            mPhotoModelProxy->rowCount(QModelIndex()) - tilesPerColRow)
        {
            newIndex = oldIndex + tilesPerColRow;

            mPhotoSelection->setCurrentIndex(mPhotoModelProxy->index(newIndex,
                0),
                QItemSelectionModel::ClearAndSelect);
            return true;
        }
    }
    return false;
}

void MainWindow::onTileDoubleClicked(const QModelIndex&)
{
    mLibrary->onShowLoupe();
}

void MainWindow::onLibraryContextMenu(const QPoint& pos)
{
    QMenu* mn = ui->menuPhoto;

    mn->exec(mLibrary->mapToGlobal(pos));
}

void MainWindow::onSelectionChanged(const QItemSelection& /*selected*/, const QItemSelection& /*deselected*/)
{
    updateInformationBar();
}

void MainWindow::onCurrentChanged(const QModelIndex& current, const QModelIndex& /*previous*/)
{
    setPhotoActionsAvailability(current.isValid());

    mDevelop->setPhoto(currentPhoto());

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
    int            c           = mPhotoModelProxy->rowCount(QModelIndex());
    QModelIndex    topLeft     = mPhotoModelProxy->index(0, 0);
    QModelIndex    bottomRight = mPhotoModelProxy->index(c - 1, 0);
    QItemSelection selection (topLeft, bottomRight);

    mPhotoSelection->select(selection, QItemSelectionModel::Select);
}

void MainWindow::onSelectNone()
{
    mPhotoSelection->clear();
}

void MainWindow::onActionRegenHashes()
{
    // for all images in the view, recalculate hashes.
    // start as background job.

    RegenHashesTask* r = new RegenHashesTask(mPhotoModelProxy->toList());

    mBackgroundTaskManager->addRunnable(r);
    r->start();
}

void MainWindow::onActionImportTriggered()
{
    ImportDialog* importDialog = new ImportDialog(this);
    int           resultCode   = importDialog->exec();

    if (resultCode == QDialog::Accepted)
    {
        ImportBackgroundTask* r = new ImportBackgroundTask(importDialog->importInfo());
        mBackgroundTaskManager->addRunnable(r);
        connect(r, &ImportBackgroundTask::taskFinished, this, &MainWindow::onImportFinished);
        r->start();
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

void MainWindow::onDeletePhotos()
{
    // TODO: if the view is in File view, offer option to delete from disk.
    // if view is in Collection view, then only offer to remove from collection

    QMessageBox msgBox;

    msgBox.setWindowTitle("Delete photos");
    msgBox.setText(QString("Do you wish to delete %1 photos?").arg(mPhotoSelection->selection().indexes().size()));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Cancel);
    QPushButton* libOnly    = msgBox.addButton("From Library Only", QMessageBox::AcceptRole);
    QPushButton* libAndDisk = msgBox.addButton("From Library and Disk", QMessageBox::DestructiveRole);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    msgBox.setEscapeButton(QMessageBox::Cancel);

    msgBox.exec();

    if (msgBox.clickedButton() == libOnly)
    {
        QList<Photo> list = mPhotoModelProxy->toList(mPhotoSelection->selection());
        mPhotoWorkUnit->deletePhotos(list, false);
        foreach(QItemSelectionRange range, mPhotoSelection->selection())
        {
            mPhotoModelProxy->removeRows(range.top(), range.height());
        }
        // TODO: consider moving this to a central place.(the work manager)
        mLibrary->reloadPathModel();
    }
    else if (msgBox.clickedButton() == libAndDisk)
    {
        QList<Photo> list = mPhotoModelProxy->toList(mPhotoSelection->selection());
        mPhotoWorkUnit->deletePhotos(list, true);
        foreach(QItemSelectionRange range, mPhotoSelection->selection())
        {
            mPhotoModelProxy->removeRows(range.top(), range.height());
        }
        mLibrary->reloadPathModel();
    } // else do nothing
}

void MainWindow::onDeleteRejectedPhotos()
{
    qDebug() << "void MainWindow::onDeleteRejectedPhotos() Not implemented";
}

void MainWindow::onShowInFileBrowser()
{
    Photo p = currentPhoto();

    if (p.isNull())
        return;
    QString pathIn = p.srcImagePath();

#if defined(Q_OS_WIN)
    const QString explorer = Environment::systemEnvironment().searchInPath(QLatin1String("explorer.exe"));

    if (explorer.isEmpty())
    {
        QMessageBox::warning(this,
            tr("Launching Windows Explorer failed"),
            tr("Could not find explorer.exe in path to launch Windows Explorer."));
        return;
    }
    QString param;

    if (!QFileInfo(pathIn).isDir())
        param = QLatin1String("/select,");
    param += QDir::toNativeSeparators(pathIn);
    QString command = explorer + " " + param;
    QProcess::startDetached(command);
#elif defined(Q_OS_MAC)
    QStringList scriptArgs;
    scriptArgs << QLatin1String("-e")
               << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"")
        .arg(pathIn);
    QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
    scriptArgs.clear();
    scriptArgs << QLatin1String("-e")
               << QLatin1String("tell application \"Finder\" to activate");
    QProcess::execute("/usr/bin/osascript", scriptArgs);
#else
    // we cannot select a file here, because no file browser really supports it...
    const QFileInfo fileInfo(pathIn);
    const QString   folder = fileInfo.absoluteFilePath();
    const QString   app    = Utils::UnixUtils::fileBrowser(Core::ICore::instance()->settings());
    QProcess        browserProc;
    const QString   browserArgs = Utils::UnixUtils::substituteFileBrowserParameters(app, folder);

    if (debug)
        qDebug() <<  browserArgs;
    bool          success = browserProc.startDetached(browserArgs);
    const QString error   = QString::fromLocal8Bit(browserProc.readAllStandardError());
    success = success && error.isEmpty();

    if (!success)
        showGraphicalShellError(this, app, error);
#endif
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

void MainWindow::onImportFinished(BackgroundTask* task)
{
    ImportBackgroundTask* t = static_cast<ImportBackgroundTask*>(task);

    QList<long long>      resultList = t->resultList();

    // remove any values from this list that are not visible (ie. do not belong to the model)
    // TODO: change subdirectory setting here
    mPhotoWorkUnit->filterList(resultList, mSourceModel->rootPath(), true);

    mSourceModel->addData(resultList);

    task->deleteLater();
    // update the files tree as well and the collection tree

    mLibrary->reloadPathModel();
}

void MainWindow::onModelReset()
{
    setPhotoActionsAvailability(false);
    updateInformationBar();
}

void MainWindow::onPhotoModelRowsInserted(const QModelIndex& /*parent*/, int /*start*/, int /*end*/)
{
    updateInformationBar();
}

void MainWindow::onPhotoModelRowsRemoved(const QModelIndex& /*parent*/, int /*start*/, int /*end*/)
{
    updateInformationBar();
}

void MainWindow::onFilterApplied(const PhotoFilterInfo& info)
{
    mPhotoModelProxy->setFilter(info);
}

void MainWindow::onPhotoSourceChanged(PhotoModel::SourceType type, long long id)
{
    mSourceModel->setRootPath(type, id);

    mPhotoModelProxy->setSortRole(Photo::DateTimeRole);
    mPhotoModelProxy->sort(0, Qt::AscendingOrder);
}

void MainWindow::onShowGrid()
{
    // force library module to the front
    onModeLibraryClicked();
    mLibrary->onShowGrid();
}

void MainWindow::onShowLoupe()
{
    // force library module to the front
    onModeLibraryClicked();
    mLibrary->onShowLoupe();
}

void MainWindow::updateInformationBar()
{
    QString info;
    int     count    = mPhotoModelProxy->rowCount(QModelIndex());
    int     selCount = mPhotoSelection->selectedIndexes().size();

    QString imagePath;
    Photo   photo = currentPhoto();

    if (!photo.isNull())
        imagePath = " " + photo.srcImagePath();
    ui->lblInformation->setText(QString::number(selCount) + "/" + QString::number(count) + imagePath);
}

Photo MainWindow::currentPhoto()
{
    QModelIndex index = mPhotoSelection->currentIndex();

    if (index.isValid())
        return mPhotoModelProxy->data(index, Widgets::TileView::ImageRole).value<Photo>();
    else
        return Photo();
}

void MainWindow::setRating(int rating)
{
    QList<Photo> list = mPhotoModelProxy->toList(mPhotoSelection->selection());
    mPhotoWorkUnit->setRating(list, rating);
    QVector<int> roles;
    roles.append(Widgets::TileView::ImageRole);
    mSourceModel->refreshData(list);
}

void MainWindow::setFlag(Photo::Flag flag)
{
    QList<Photo> list = mPhotoModelProxy->toList(mPhotoSelection->selection());
    mPhotoWorkUnit->setFlag(list, flag);
    QVector<int> roles;
    roles.append(Widgets::TileView::ImageRole);
    mSourceModel->refreshData(list);
}

void MainWindow::setColorLabel(Photo::ColorLabel color)
{
    QList<Photo> list = mPhotoModelProxy->toList(mPhotoSelection->selection());
    mPhotoWorkUnit->setColorLabel(list, color);
    QVector<int> roles;
    roles.append(Widgets::TileView::ImageRole);
    mSourceModel->refreshData(list);
}

void MainWindow::setPhotoActionsAvailability(bool enabled)
{
    mActionStatePhoto.enableAll(enabled);
}
}
