#include <QInputDialog>

#include "library.h"
#include "ui_library.h"

#include "constants.h"
#include "dragdropinfo.h"

namespace PhotoStage {
Library::Library(PhotoSortFilterProxyModel* const model, QWidget* parent)
    : Module(parent), ui(new Ui::Library), mPhotoModel(model),
      mFontAccessFoundIcons(QFont("Accessibility Foundicons", 15)),
      mSelectionModel(NULL)
{
  ui->setupUi(this);

  QSettings settings;
  settings.beginGroup(SETTINGS_GROUP_LIBRARY);
  QList<int> l;

  if (settings.contains(SETTINGS_SPLITTER_LIBRARY_SIZES))
  {
    foreach (QVariant v,
             settings.value(SETTINGS_SPLITTER_LIBRARY_SIZES).toList())
    {
      l << v.toInt();
    }
  }
  else
  {
    l << 200 << 600 << 200;
  }
  ui->splitterMain->setSizes(l);
  settings.endGroup();

  ImageDbTile* tile = new ImageDbTile(ui->mPhotoGrid);
  // connect(tile,&ImageDbTile::rotateLeftClicked, this,
  // &Library::rotateLeftClicked);
  // connect(tile,&ImageDbTile::rotateRightClicked, this,
  // &Library::rotateRightClicked);
  // connect(tile,&ImageDbTile::ratingClicked,this,&Library::ratingClicked);

  ui->mPhotoGrid->setDelegate(tile);
  ui->mPhotoGrid->setMinimumCellWidth(150);
  ui->mPhotoGrid->setMaximumCellWidth(200);
  ui->mPhotoGrid->setCheckBoxMode(false);

  ui->mPhotoGrid->setDragEnabled(true);
  ui->mPhotoGrid->setAcceptDrops(false);

  // ui->mClvPhotos->setTilesPerColRow(ui->hsThumbSize->value());
  // ui->mClvPhotos->setObjectName("LibaryPhotos");

  ui->mPhotoGrid->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ui->mPhotoGrid, &Widgets::TileView::customContextMenuRequested, this,
          &Library::onCustomContextMenu);
  connect(ui->mPhotoGrid, &Widgets::TileView::doubleClickTile, this,
          &Library::onTileDoubleClicked);
  connect(ui->mPhotoGrid, &Widgets::TileView::visibleTilesChanged,
          (PhotoModel*)mPhotoModel->sourceModel(),
          &PhotoModel::onVisibleTilesChanged);

  // These models are auto deleted by the QObject hierarchy
  ui->mPhotoGrid->setModel(mPhotoModel);

  connect(mPhotoModel, &PhotoSortFilterProxyModel::dataChanged, this,
          &Library::onDataChanged);

  ui->scrollArea->setWidgetResizable(true);
  ui->scrollArea_2->setWidgetResizable(true);

  // **** MODULES LEFT

  // Filter module

  FilterModule* fm = new FilterModule(ui->ModulePanel_1);
  ui->ModulePanel_1->addPanel(tr("Filter"), fm);
  connect(fm, &FilterModule::modelFilterApplied, this,
          &Library::modelFilterApplied);

  QMenu* menu;

  // shortcuts module
  menu = new QMenu(this);
  menu->addAction("New Collection", this, SLOT(onNewCollection()));
  menu->addAction("New Work Collection", this, SLOT(onNewWorkCollection()));
  mShortcutModule = new ShortcutModule(ui->ModulePanel_1);
  ui->ModulePanel_1->addPanel(tr("Ad-hoc Collections"), mShortcutModule,
                              QString(), menu);
  connect(mShortcutModule, &ShortcutModule::workCollectionSelected, this,
          &Library::onWorkCollectionSelected);
  connect(mShortcutModule, &ShortcutModule::importCollectionSelected, this,
          &Library::onImportCollectionSelected);

  // Files module
  mFilesModule = new FilesModule(ui->ModulePanel_1);
  ui->ModulePanel_1->addPanel(tr("Folders"), mFilesModule);
  connect(mFilesModule, &FilesModule::pathSelected, this,
          &Library::onPathSelected);

  // collections module
  mCollectionModule = new CollectionModule(ui->ModulePanel_1);

  menu = new QMenu(this);
  menu->addAction(tr("New Collection"), this, SLOT(onNewCollection()));
  ui->ModulePanel_1->addPanel(tr("Collections"), mCollectionModule, QString(),
                              menu);
  connect(mCollectionModule, &CollectionModule::collectionSelected, this,
          &Library::onCollectionSelected);

  // **** MODULES RIGHT

  mHistogramModule = new LibraryHistogramModule(ui->ModulePanel_2);
  ui->ModulePanel_2->addPanel(tr("Histogram"), mHistogramModule);

  // Tagging (editing keywords module)
  mTagging = new TaggingModule(ui->ModulePanel_2);
  ui->ModulePanel_2->addPanel(tr("Tagging"), mTagging);

  // **** MODULE
  // Keyword list module
  mKeywordModule = new KeywordModule(ui->ModulePanel_2);
  ui->ModulePanel_2->addPanel(tr("Keyword List"), mKeywordModule);

  mMetaDataModule = new MetaDataModule(ui->ModulePanel_2);
  ui->ModulePanel_2->addPanel(tr("Meta Data"), mMetaDataModule);

  onShowGrid();

  mPhotoWorkUnit = DatabaseAccess::photoDao();
}

Library::~Library()
{
  QSettings settings;

  settings.beginGroup(SETTINGS_GROUP_LIBRARY);
  QVariantList list;

  foreach (int size, ui->splitterMain->sizes())
  {
    list << size;
  }
  settings.setValue(SETTINGS_SPLITTER_LIBRARY_SIZES, list);

  settings.endGroup();
  delete ui;
}

QRect Library::lightGap()
{
  QPoint pos = ui->mPhotoGrid->mapToGlobal(QPoint(0, 0));
  QRect  gap = QRect(pos, ui->mPhotoGrid->size());

  return gap;
}

bool Library::canSelectionChange()
{
  //    qDebug() << qApp->focusWidget()->objectName();
  return ui->mPhotoGrid->hasFocus() || ui->mLoupeScrollView->hasFocus();
}

bool Library::canSelectUpDown()
{
  return ui->mPhotoGrid->isVisible();
}

int Library::tilesPerRowOrCol()
{
  return ui->mPhotoGrid->tilesPerColRow();
}

void Library::setSelectionModel(QItemSelectionModel* selectionModel)
{
  mSelectionModel = selectionModel;
  ui->mPhotoGrid->setSelectionModel(selectionModel);
  connect(selectionModel, &QItemSelectionModel::selectionChanged, this,
          &Library::onPhotoSelectionChanged);
  connect(selectionModel, &QItemSelectionModel::currentChanged, this,
          &Library::onCurrentPhotoChanged);
}

void Library::onCustomContextMenu(const QPoint& pos)
{
  emit customContextMenuRequested(ui->mPhotoGrid->mapTo(this, pos));
}

void Library::onZoomLevelChanged(int zoomLevel)
{
  Widgets::LoupeView::ZoomMode zoom[Widgets::LoupeView::ZoomLast] = {
      Widgets::LoupeView::ZoomFit, Widgets::LoupeView::Zoom25,
      Widgets::LoupeView::Zoom50,  Widgets::LoupeView::Zoom100,
      Widgets::LoupeView::Zoom150, Widgets::LoupeView::Zoom200,
      Widgets::LoupeView::Zoom300, Widgets::LoupeView::Zoom400,
      Widgets::LoupeView::Zomm800};

  ui->mLoupeView->setZoomMode(zoom[zoomLevel]);
}

void Library::onPathSelected(long long pathid)
{
  mCollectionModule->sourceChanged(PhotoModel::SourceFiles);
  mShortcutModule->sourceChanged(PhotoModel::SourceFiles);
  emit photoSourceChanged(PhotoModel::SourceFiles, pathid);
}

void Library::onCollectionSelected(long long collectionid)
{
  mShortcutModule->sourceChanged(PhotoModel::SourceCollectionUser);
  mFilesModule->sourceChanged(PhotoModel::SourceCollectionUser);
  emit photoSourceChanged(PhotoModel::SourceCollectionUser, collectionid);
}

void Library::onWorkCollectionSelected(long long id)
{
  mCollectionModule->sourceChanged(PhotoModel::SourceCollectionWork);
  mFilesModule->sourceChanged(PhotoModel::SourceCollectionWork);
  mShortcutModule->sourceChanged(PhotoModel::SourceCollectionWork);
  emit photoSourceChanged(PhotoModel::SourceCollectionWork, id);
}

void Library::onImportCollectionSelected(long long id)
{
  mCollectionModule->sourceChanged(PhotoModel::SourceCollectionImport);
  mFilesModule->sourceChanged(PhotoModel::SourceCollectionImport);
  mShortcutModule->sourceChanged(PhotoModel::SourceCollectionImport);
  emit photoSourceChanged(PhotoModel::SourceCollectionImport, id);
}

void Library::onThumbSizeChanged(int newValue)
{
  int w = ui->mPhotoGrid->width();

  qDebug() << "Newvalue=" << newValue;

  int newMin = w / (newValue + 1);

  if (newMin < 50)
    newMin = 50;

  ui->mPhotoGrid->setMinimumCellWidth(newMin);
}

/*void Library::rotateLeftClicked(const QModelIndex &index)
   {
    // todo: must check if there is a selection in the view.
    qDebug() << "Left Clicked";
   }

   void Library::rotateRightClicked(const QModelIndex &index)
   {
    // todo: must check if there is a selection in the view.
    qDebug() << "Right clicked";
   }
 */
/*
   void Library::ratingClicked(const QModelIndex &index, int rating)
   {
    // todo: must check if there is a selection in the view.
    qDebug() << "Rating" << rating << "set for item" << index.row();
    QVariant variant = mPhotoModel->data(index,TileView::PhotoRole);
    SqlPhotoInfo info = variant.value<SqlPhotoInfo>();
    mPhotoWorkUnit->setRating(info.id,rating);
    QVector<int> roles;
    roles.append(TileView::PhotoRole);
    mPhotoModel->updateData(index);
   }
 */

// Called when the selection in the photo tile view changes.
// get the new selectionlist and pass it to all the modules.
void Library::onPhotoSelectionChanged(const QItemSelection& selected,
                                      const QItemSelection& /*deselected*/)
{
  QList<Photo> photos;
  foreach (QModelIndex index, selected.indexes())
  {
    photos.append(mPhotoModel->data(index, Photo::DataRole).value<Photo>());
  }
  mTagging->setPhotos(photos);
}

void Library::onCurrentPhotoChanged(const QModelIndex& current,
                                    const QModelIndex& /*previous*/)
{
  QVariant data = mPhotoModel->data(current, Photo::DataRole);
  Photo    photo;

  if (data.isValid())
    photo = data.value<Photo>();

  mCurrentPhoto = photo;
  mHistogramModule->setPhoto(photo);
  mMetaDataModule->setPhoto(photo);

  if (ui->mLoupeScrollView->isVisible())
    onShowLoupe();
}

void Library::onTileDoubleClicked(const QModelIndex& /*index*/)
{
  onShowLoupe();
}

void Library::onDataChanged(const QModelIndex& /*topLeft*/,
                            const QModelIndex& /*bottomRight*/,
                            const QVector<int>& roles)
{
  if (roles.contains(Photo::DataRole))
  {
    mHistogramModule->setPhoto(mCurrentPhoto);
    mMetaDataModule->setPhoto(mCurrentPhoto);
  }
}

void Library::onCycleLoupeInfo()
{
  ui->mLoupeView->cycleInfoMode();
}

void Library::onNewCollection()
{
  mCollectionModule->onNewCollection();
}

void Library::onNewWorkCollection()
{
  bool    ok;
  QString name = QInputDialog::getText(this, "New Work Collection", "Name",
                                       QLineEdit::Normal, QString(), &ok);

  if (ok && !name.isEmpty())
  {
    DatabaseAccess::collectionDao()->addWorkCollection(name);
  }
}

void Library::onShowLoupe()
{
  if (!mCurrentPhoto.isNull())
  {
    ui->mLoupeView->setPhoto(mCurrentPhoto);
    ui->StackedWidget_1->setCurrentWidget(ui->mLoupeScrollView);
  }
}

void Library::onShowGrid()
{
  ui->StackedWidget_1->setCurrentWidget(ui->mPhotoGrid);
}

void Library::onSortKeyChanged(int key)
{
  switch (key)
  {
    case 0:
      mPhotoModel->setSortRole(Photo::DateTimeRole);
      break;

    case 1:
      mPhotoModel->setSortRole(Photo::FilenameRole);
      break;
  }
}

void Library::onSortOrderChanged()
{
  Qt::SortOrder order = mPhotoModel->sortOrder();

  if (order == Qt::AscendingOrder)
  {
    mPhotoModel->sort(0, Qt::DescendingOrder);
    ui->pbSortOrder->setText("↓");
  }
  else
  {
    mPhotoModel->sort(0, Qt::AscendingOrder);
    ui->pbSortOrder->setText("↑");
  }
}
} // namespace PhotoStage
