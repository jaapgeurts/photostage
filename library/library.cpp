#include <QInputDialog>

#include "library.h"
#include "ui_library.h"

// models
#include "sqlkeywordmodel.h"

#include "dragdropinfo.h"
#include "constants.h"

namespace PhotoStage
{
Library::Library(PhotoSortFilterProxyModel* const model, QWidget* parent) :
    Module(parent),
    ui(new Ui::Library),
    mPhotoModel(model),
    mFontAccessFoundIcons(QFont("Accessibility Foundicons", 15)),
    mSelectionModel(NULL)
{
    ui->setupUi(this);

    QSettings  settings;
    settings.beginGroup("library");
    QList<int> l;

    if (settings.contains(SETTINGS_SPLITTER_LIBRARY_SIZES))
    {
        foreach(QVariant v, settings.value(SETTINGS_SPLITTER_LIBRARY_SIZES).toList())
        {
            l << v.toInt();
        }
    }
    else
    {
        l << 200 << 600 << 200;
    }
    ui->splitterMain->setSizes(l);

    ImageDbTile* tile = new ImageDbTile(ui->mPhotoGrid);
    // connect(tile,&ImageDbTile::rotateLeftClicked, this, &Library::rotateLeftClicked);
    // connect(tile,&ImageDbTile::rotateRightClicked, this, &Library::rotateRightClicked);
    // connect(tile,&ImageDbTile::ratingClicked,this,&Library::ratingClicked);

    ui->mPhotoGrid->setTileFlyweight(tile);
    ui->mPhotoGrid->setMinimumCellWidth(150);
    ui->mPhotoGrid->setMaximumCellWidth(200);
    ui->mPhotoGrid->setCheckBoxMode(false);

    ui->mPhotoGrid->setDragEnabled(true);
    ui->mPhotoGrid->setAcceptDrops(false);

    //ui->mClvPhotos->setTilesPerColRow(ui->hsThumbSize->value());
    // ui->mClvPhotos->setObjectName("LibaryPhotos");

    ui->mPhotoGrid->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->mPhotoGrid, &Widgets::TileView::customContextMenuRequested, this, &Library::onCustomContextMenu);
    connect(ui->mPhotoGrid, &Widgets::TileView::doubleClickTile, this, &Library::onTileDoubleClicked);
    connect(ui->mPhotoGrid, &Widgets::TileView::visibleTilesChanged,
        (PhotoModel*)mPhotoModel->sourceModel(), &PhotoModel::onVisibleTilesChanged);

    // These models are auto deleted by the QObject hierarchy
    ui->mPhotoGrid->setModel(mPhotoModel);

    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea_2->setWidgetResizable(true);

    // **** MODULES LEFT

    // Filter module

    FilterModule* fm = new FilterModule(ui->ModulePanel_1);
    ui->ModulePanel_1->addPanel("Filter", fm);
    connect(fm, &FilterModule::modelFilterApplied, this, &Library::modelFilterApplied);

    QMenu* menu;

    // shortcuts module
    menu = new QMenu(this);
    menu->addAction("New Collection", this, SLOT(onNewCollection()));
    menu->addAction("New Work Collection", this, SLOT(onNewWorkCollection()));
    ShortcutModule* sm = new ShortcutModule(ui->ModulePanel_1);
    ui->ModulePanel_1->addPanel("Shortcuts", sm, menu);
    connect(sm, &ShortcutModule::collectionSelected, this, &Library::onCollectionSelected);

    // Files module
    mFilesModule = new FilesModule(ui->ModulePanel_1);
    ui->ModulePanel_1->addPanel("Folders", mFilesModule);
    connect(mFilesModule, &FilesModule::pathSelected, this, &Library::onPathSelected);

    // collections module
    mCollectionModule = new CollectionModule(ui->ModulePanel_1);

    menu = new QMenu(this);
    menu->addAction("New Collection", this, SLOT(onNewCollection()));
    ui->ModulePanel_1->addPanel("Collections", mCollectionModule, menu);
    connect(mCollectionModule, &CollectionModule::collectionSelected, this, &Library::onCollectionSelected);

    // **** MODULES RIGHT

    mHistogramModule = new LibraryHistogramModule(ui->ModulePanel_2);
    ui->ModulePanel_2->addPanel("Histogram", mHistogramModule);

    // Keywording (editing keywords module)
    mKeywording = new TaggingModule(ui->ModulePanel_2);
    ui->ModulePanel_2->addPanel("Keywords", mKeywording);

    // **** MODULE
    // Keyword list module
    Widgets::FixedTreeView* trvwKeywords = new Widgets::FixedTreeView(ui->ModulePanel_2);
    SqlKeywordModel*        keywordModel = new SqlKeywordModel(this);
    trvwKeywords->setModel(keywordModel);
    trvwKeywords->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->ModulePanel_2->addPanel("Keyword List", trvwKeywords);

    mMetaDataModule = new MetaDataModule(ui->ModulePanel_2);
    ui->ModulePanel_2->addPanel("Meta Data", mMetaDataModule);

    onShowGrid();

    mPhotoWorkUnit = DatabaseAccess::photoDao();
}

Library::~Library()
{
    QSettings settings;

    settings.beginGroup("library");
    QVariantList list;

    foreach(int size, ui->splitterMain->sizes())
    {
        list << size;
    }
    settings.setValue(SETTINGS_SPLITTER_LIBRARY_SIZES, list);

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
    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &Library::onPhotoSelectionChanged);
    connect(selectionModel, &QItemSelectionModel::currentChanged, this, &Library::onCurrentPhotoChanged);
}

void Library::onCustomContextMenu(const QPoint& pos)
{
    emit customContextMenuRequested(ui->mPhotoGrid->mapTo(this, pos));
}

void Library::onZoomLevelChanged(int zoomLevel)
{
    Widgets::LoupeView::ZoomMode zoom[Widgets::LoupeView::ZoomLast] = {
        Widgets::LoupeView::ZoomFit, Widgets::LoupeView::Zoom25, Widgets::LoupeView::Zoom50,
        Widgets::LoupeView::Zoom100, Widgets::LoupeView::Zoom150, Widgets::LoupeView::Zoom200,
        Widgets::LoupeView::Zoom300, Widgets::LoupeView::Zoom400, Widgets::LoupeView::Zomm800
    };

    ui->mLoupeView->setZoomMode(zoom[zoomLevel]);
}

void Library::onPathSelected(long long pathid)
{
    emit photoSourceChanged(PhotoModel::SourceFiles, pathid);
}

void Library::onCollectionSelected(long long collectionid)
{
    emit photoSourceChanged(PhotoModel::SourceCollection, collectionid);
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
void Library::onPhotoSelectionChanged(const QItemSelection& selected, const QItemSelection& /*deselected*/)
{
    QList<Photo> photos;
    foreach (QModelIndex index, selected.indexes())
    photos.append(mPhotoModel->data(index, Widgets::TileView::ImageRole).value<Photo>());

    mKeywording->setPhotos(photos);
}

void Library::onCurrentPhotoChanged(const QModelIndex& current, const QModelIndex& /*previous*/)
{
    if (!current.isValid())
    {
        Photo p;
        mHistogramModule->setPhoto(p);
        mMetaDataModule->setPhoto(p);
    }
    Photo photo = mPhotoModel->data(current, Widgets::TileView::ImageRole).value<Photo>();

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
    QString name = QInputDialog::getText(this, "New Work Collection", "Name", QLineEdit::Normal, QString(), &ok);

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
}
