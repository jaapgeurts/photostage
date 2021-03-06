#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QMessageBox>
#include <QTextEdit>
#include <QTreeView>
#include <QVariantList>

// Models
#include "photo.h"
#include "photomodel.h"

// dialogs
#include "aboutdialog.h"
#include "import/importdialog.h"
#include "preferencesdialog.h"
#include "timeadjustdialog.h"

// widget related stuf
#include "filmstriptile.h"
#include "widgets/backgroundtaskprogress.h"
#include "widgets/translucentwindow.h"

#include "backgroundtask.h"
#include "import/importbackgroundtask.h"
#include "regenhashestask.h"

#include "constants.h"

namespace PhotoStage {
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
  QApplication::setFont(QFont(QString("verdana"), 10));

  ui->setupUi(this);

  mDatabaseAccess = DatabaseAccess::instance(this);

  QSettings settings;

  QList<int> l;
  settings.beginGroup(SETTINGS_GROUP_MAINWINDOW);
  move(settings.value(SETTINGS_WINDOW_LOCATION).toPoint());

  if (settings.contains(SETTINGS_SPLITTER_FILMSTRIP_SIZES))
  {
    foreach (QVariant v,
             settings.value(SETTINGS_SPLITTER_FILMSTRIP_SIZES).toList())
    {
      l << v.toInt();
    }
  }
  else
  {
    l << 600 << 200;
  }
  ui->splitter->setSizes(l);

  settings.endGroup();

  mSourceModel     = new PhotoModel(this);
  mPhotoModelProxy = new PhotoSortFilterProxyModel(this);
  mPhotoModelProxy->setDynamicSortFilter(false);
  mPhotoModelProxy->setSourceModel(mSourceModel);

  // setup PhotoModel connections
  connect(mSourceModel, &PhotoModel::modelReset, this,
          &MainWindow::onModelReset);
  connect(mSourceModel, &PhotoModel::rowsInserted, this,
          &MainWindow::onPhotoModelRowsInserted);
  connect(mSourceModel, &PhotoModel::rowsRemoved, this,
          &MainWindow::onPhotoModelRowsRemoved);
  connect(mSourceModel, &PhotoModel::dataChanged, this,
          &MainWindow::onPhotoModelDataChanged);

  mPhotoSelection = new QItemSelectionModel(mPhotoModelProxy, this);
  // set up connections
  connect(mPhotoSelection, &QItemSelectionModel::selectionChanged, this,
          &MainWindow::onSelectionChanged);
  connect(mPhotoSelection, &QItemSelectionModel::currentChanged, this,
          &MainWindow::onCurrentChanged);

  //**************
  // create the LIBRARY MODULE
  mLibrary = new Library(mPhotoModelProxy, this);
  mLibrary->setSelectionModel(mPhotoSelection);
  ui->stackedWidget->addWidget(mLibrary);

  // setup connections
  connect(mLibrary, &Library::photoSourceChanged, this,
          &MainWindow::onPhotoSourceChanged);
  connect(mLibrary, &Library::modelFilterApplied, this,
          &MainWindow::onFilterApplied);
  connect(mLibrary, &Library::customContextMenuRequested, this,
          &MainWindow::onLibraryContextMenu);

  connect(ui->actionNewCollection, &QAction::triggered, mLibrary,
          &Library::onNewCollection);
  connect(ui->actionNewWorkCollection, &QAction::triggered, mLibrary,
          &Library::onNewWorkCollection);

  connect(ui->actionLoupeInfoCycle, &QAction::triggered, mLibrary,
          &Library::onCycleLoupeInfo);

  ui->filmStrip->setModel(mPhotoModelProxy);
  ui->filmStrip->setDragEnabled(true);
  ui->filmStrip->setAcceptDrops(false);
  FilmstripTile* fsTile = new FilmstripTile(ui->filmStrip);
  ui->filmStrip->setDelegate(fsTile);
  ui->filmStrip->setMinimumCellHeight(80);
  ui->filmStrip->setTilesPerColRow(1);
  ui->filmStrip->setCheckBoxMode(false);
  ui->filmStrip->setOrientation(Qt::Horizontal);
  ui->filmStrip->setSelectionModel(mPhotoSelection);
  ui->filmStrip->setObjectName("Filmstrip");

  connect(ui->filmStrip, &Widgets::TileView::doubleClickTile, this,
          &MainWindow::onTileDoubleClicked);
  connect(ui->filmStrip, &Widgets::TileView::visibleTilesChanged, mSourceModel,
          &PhotoModel::onVisibleTilesChanged);

  //***************
  // Create the DEVELOP MODULE
  mDevelop = new Develop(this);
  ui->stackedWidget->addWidget(mDevelop);

  // ***************
  // Create the MAP MODULE
  mMap = new Cartography(mPhotoModelProxy, this);
  ui->stackedWidget->addWidget(mMap);
  mMap->setSelectionModel(mPhotoSelection);

  // Set the current module variables
  mCurrentModule = mLibrary;
  ui->stackedWidget->setCurrentWidget(mLibrary);

  mPhotoWorkUnit = DatabaseAccess::photoDao();

  mBackgroundTaskManager =
      new BackgroundTaskManager(ui->scrollAreaWidgetContents, this);

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

  mActionStatePhoto.addAction(ui->actionAddToWorkCollection);

  mActionStatePhoto.addAction(ui->actionEdit_Capture_Time);

  mActionStatePhoto.disableAll(true);

  qApp->installEventFilter(this);
}

MainWindow::~MainWindow()
{
  mBackgroundTaskManager->cancelAll();
  // QDesktopWidget * desktop = QApplication::desktop();
  QSettings settings;
  settings.beginGroup(SETTINGS_GROUP_MAINWINDOW);

  settings.setValue(SETTINGS_WINDOW_LOCATION, pos());
  QVariantList list;
  foreach (int size, ui->splitter->sizes())
  {
    list << size;
  }
  settings.setValue(SETTINGS_SPLITTER_FILMSTRIP_SIZES, list);
  settings.endGroup();
  delete ui;
}

bool MainWindow::eventFilter(QObject* /*obj*/, QEvent* event)
// bool MainWindow::event(QEvent* event)
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

        if (!mLibrary->canSelectionChange() && !ui->filmStrip->hasFocus())
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
    int dlgResult = QMessageBox::warning(
        this, "Running active tasks",
        "There are active tasks running. Do you wish to stop them and quit?",
        "Stop tasks and quit", "Keep running", QString(), 1);

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

    if (index.row() < mPhotoModelProxy->rowCount(QModelIndex()) - 1)
    {
      newIndex = oldIndex + 1;
      mPhotoSelection->setCurrentIndex(mPhotoModelProxy->index(newIndex, 0),
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
      mPhotoSelection->setCurrentIndex(mPhotoModelProxy->index(newIndex, 0),
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

      mPhotoSelection->setCurrentIndex(mPhotoModelProxy->index(newIndex, 0),
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

    if (oldIndex < mPhotoModelProxy->rowCount(QModelIndex()) - tilesPerColRow)
    {
      newIndex = oldIndex + tilesPerColRow;

      mPhotoSelection->setCurrentIndex(mPhotoModelProxy->index(newIndex, 0),
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

void MainWindow::onSelectionChanged(const QItemSelection& /*selected*/,
                                    const QItemSelection& /*deselected*/)
{
  updateInformationBar();
}

void MainWindow::onCurrentChanged(const QModelIndex& current,
                                  const QModelIndex& previous)
{
  setPhotoActionsAvailability(current.isValid());

  // Remove the Orginal image from the previous current photo to release memory.
  QVariant data = mPhotoModelProxy->data(previous, Photo::DataRole);

  if (!data.isNull())
  {
    Photo p = data.value<Photo>();

    if (!p.isNull())
    {
      p.setOriginalImage(Image());
      p.setDevelopPreviewsRGB(QImage());
    }
  }

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
  QItemSelection selection(topLeft, bottomRight);

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
    ImportBackgroundTask* r =
        new ImportBackgroundTask(importDialog->importInfo());
    mBackgroundTaskManager->addRunnable(r);
    connect(r, &ImportBackgroundTask::taskFinished, this,
            &MainWindow::onImportFinished);
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

  timeAdjustDialog->setPhoto(currentPhoto());

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

  if (mSourceModel->rootSource() == PhotoModel::SourceCollectionImport)
  {
    msgBox.setText(tr("You can't delete photo's from an import collection. You "
                      "can delete the photo's from disk through the files view "
                      "or delete the whole collection."));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setEscapeButton(QMessageBox::Ok);
    msgBox.exec();
    return;
  }

  QPushButton* libAndDisk = NULL;
  QPushButton* libOnly    = NULL;

  msgBox.setText(tr("Do you wish to delete %1 photos?")
                     .arg(mPhotoSelection->selection().indexes().size()));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Cancel);

  QString deleteAction = tr("From Collection only");
  QString windowTitle  = tr("Remove photos");

  if (mSourceModel->rootSource() == PhotoModel::SourceFiles)
  {
    libAndDisk   = msgBox.addButton(tr("From Library and Disk"),
                                  QMessageBox::DestructiveRole);
    deleteAction = tr("From Library Only");
    windowTitle  = tr("Delete photos");
  }

  msgBox.setWindowTitle(windowTitle);

  libOnly = msgBox.addButton(deleteAction, QMessageBox::AcceptRole);

  msgBox.setDefaultButton(QMessageBox::Cancel);
  msgBox.setEscapeButton(QMessageBox::Cancel);

  msgBox.exec();

  if (msgBox.clickedButton() == libOnly)
  {
    QList<Photo> list = mPhotoModelProxy->toList(mPhotoSelection->selection());

    if (mSourceModel->rootSource() == PhotoModel::SourceFiles)
      mPhotoWorkUnit->deletePhotos(list, false);
    else if (mSourceModel->rootSource() == PhotoModel::SourceCollectionUser)
      DatabaseAccess::collectionDao()->removePhotosFromCollection(
          mSourceModel->rootId(), list);
  }
  else if (msgBox.clickedButton() == libAndDisk &&
           mSourceModel->rootSource() == PhotoModel::SourceFiles)
  {
    QList<Photo> list = mPhotoModelProxy->toList(mPhotoSelection->selection());
    mPhotoWorkUnit->deletePhotos(list, true);
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
  const QString explorer = Environment::systemEnvironment().searchInPath(
      QLatin1String("explorer.exe"));

  if (explorer.isEmpty())
  {
    QMessageBox::warning(
        this, tr("Launching Windows Explorer failed"),
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
             << QString::fromLatin1(
                    "tell application \"Finder\" to reveal POSIX file \"%1\"")
                    .arg(pathIn);
  QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
  scriptArgs.clear();
  scriptArgs << QLatin1String("-e")
             << QLatin1String("tell application \"Finder\" to activate");
  QProcess::execute("/usr/bin/osascript", scriptArgs);
#elif defined(Q_OS_LINUX)
  // TODO: reveal file in system browser
  // we cannot select a file here, because no file browser really supports it...

  /*  const QFileInfo fileInfo(pathIn);
    const QString   folder = fileInfo.absoluteFilePath();
    const QString   app    =
    Utils::UnixUtils::fileBrowser(Core::ICore::instance()->settings()); QProcess
    browserProc; const QString   browserArgs =
    Utils::UnixUtils::substituteFileBrowserParameters(app, folder);

    if (debug)
        qDebug() <<  browserArgs;
    bool          success = browserProc.startDetached(browserArgs);
    const QString error   =
    QString::fromLocal8Bit(browserProc.readAllStandardError()); success =
    success && error.isEmpty();
*/
  //    if (!success)
  //  showGraphicalShellError(this, app, error);
  throw new std::runtime_error("NOT IMPLEMENTED");
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
  // w->showFullScreen();
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

    // w->showMaximized();
    // w->showFullScreen();
  }
}

void MainWindow::onImportFinished(BackgroundTask* task)
{
  task->deleteLater();
  // update the files tree as well and the collection tree
}

void MainWindow::onModelReset()
{
  setPhotoActionsAvailability(false);
  updateInformationBar();
}

void MainWindow::onPhotoModelRowsInserted(const QModelIndex& /*parent*/,
                                          int /*start*/, int /*end*/)
{
  updateInformationBar();
}

void MainWindow::onPhotoModelRowsRemoved(const QModelIndex& /*parent*/,
                                         int /*start*/, int /*end*/)
{
  updateInformationBar();
}

void MainWindow::onPhotoModelDataChanged(const QModelIndex& /*topLeft*/,
                                         const QModelIndex& /*bottomRight*/,
                                         const QVector<int>& /*roles*/)
{
  // update the develop module in case a photo was reloaded
  mDevelop->onPhotoUpdated();
}

void MainWindow::onFilterApplied(const PhotoFilterInfo& info)
{
  mPhotoModelProxy->setFilter(info);
}

void MainWindow::onPhotoSourceChanged(PhotoModel::SourceType type, long long id)
{
  mSourceModel->setRootSourceId(type, id);

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
  ui->lblInformation->setText(QString::number(selCount) + "/" +
                              QString::number(count) + imagePath);
}

Photo MainWindow::currentPhoto()
{
  QModelIndex index = mPhotoSelection->currentIndex();

  if (index.isValid())
    return mPhotoModelProxy->data(index, Photo::DataRole).value<Photo>();
  else
    return Photo();
}

void MainWindow::setRating(int rating)
{
  QList<Photo> list = mPhotoModelProxy->toList(mPhotoSelection->selection());
  mPhotoWorkUnit->setRating(list, rating);
  //    QVector<int> roles;
  //    roles << Photo::DataRole;
  mSourceModel->refreshData(list);
}

void MainWindow::setFlag(Photo::Flag flag)
{
  QList<Photo> list = mPhotoModelProxy->toList(mPhotoSelection->selection());
  mPhotoWorkUnit->setFlag(list, flag);
  //    QVector<int> roles;
  //    roles << Photo::DataRole;
  mSourceModel->refreshData(list);
}

void MainWindow::setColorLabel(Photo::ColorLabel color)
{
  QList<Photo> list = mPhotoModelProxy->toList(mPhotoSelection->selection());
  mPhotoWorkUnit->setColorLabel(list, color);
  //    QVector<int> roles;
  //    roles << Photo::DataRole;
  mSourceModel->refreshData(list);
}

void MainWindow::setPhotoActionsAvailability(bool enabled)
{
  mActionStatePhoto.enableAll(enabled);
}
} // namespace PhotoStage
