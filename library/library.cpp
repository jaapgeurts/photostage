#include <QMenu>

#include "library.h"
#include "ui_library.h"

// models
#include "sqlkeywordmodel.h"

#define SETTINGS_SPLITTER_LIBRARY_SIZES "splitter_main"
#define SETTINGS_LIBRARY_FILES_PATHITEM "files/pathitem"

namespace PhotoStage
{
Library::Library(QSortFilterProxyModel* const model, QWidget* parent) :
    Module(parent),
    ui(new Ui::Library),
    mPhotoModel(model),
    mFontAccessFoundIcons(QFont("Accessibility Foundicons", 15))
{
    ui->setupUi(this);

    QSettings  settings;
    settings.beginGroup("library");
    QList<int> l;

    if (settings.contains(SETTINGS_SPLITTER_LIBRARY_SIZES))
    {
        foreach(QVariant v, settings.value(
                SETTINGS_SPLITTER_LIBRARY_SIZES).toList())
        {
            l << v.toInt();
        }
    }
    else
    {
        l << 200 << 600 << 200;
    }
    ui->splitterMain->setSizes(l);

    ImageDbTile* tile = new ImageDbTile(ui->mClvPhotos);
    // connect(tile,&ImageDbTile::rotateLeftClicked, this, &Library::rotateLeftClicked);
    // connect(tile,&ImageDbTile::rotateRightClicked, this, &Library::rotateRightClicked);
    // connect(tile,&ImageDbTile::ratingClicked,this,&Library::ratingClicked);

    ui->mClvPhotos->setTileFlyweight(tile);
    ui->mClvPhotos->setMinimumCellWidth(150);
    ui->mClvPhotos->setMaximumCellWidth(200);
    ui->mClvPhotos->setCheckBoxMode(false);
    //ui->mClvPhotos->setTilesPerColRow(ui->hsThumbSize->value());
    // ui->mClvPhotos->setObjectName("LibaryPhotos");

    ui->mClvPhotos->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->mClvPhotos, &TileView::TileView::customContextMenuRequested,
        this, &Library::customContextMenu);
    connect(ui->mClvPhotos, &TileView::TileView::doubleClickTile,
        this, &Library::onTileDoubleClicked);

    // These models are auto deleted by the QObject hierarchy
    ui->mClvPhotos->setModel(mPhotoModel);

    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea_2->setWidgetResizable(true);

    // **** MODULES LEFT

    // Filter module

    FilterModule* fm = new FilterModule(ui->ModulePanel_1);
    ui->ModulePanel_1->addPanel("Filter", fm);
    connect(fm, &FilterModule::modelFilterApplied,
        this, &Library::modelFilterApplied);

    // shortcuts module
    ShortcutModule* sm = new ShortcutModule(ui->ModulePanel_1);
    ui->ModulePanel_1->addPanel("Shortcuts", sm);

    // Files module
    mTrvwFiles = new FixedTreeView(ui->ModulePanel_1);
    mPathModel = new SqlPathModel(this);
    mTrvwFiles->setModel(mPathModel);
    mTrvwFiles->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->ModulePanel_1->addPanel("Folders", mTrvwFiles);
    connect(mTrvwFiles, &FixedTreeView::clicked, this,
        &Library::onFilesClicked);
    connect(mPathModel, &SqlPathModel::rowsInserted, this,
        &Library::onPathModelRowsAdded);
    connect(mPathModel, &SqlPathModel::rowsRemoved, this,
        &Library::onPathModelRowsRemoved);

    // collections module
    CollectionModule* cm   = new CollectionModule(ui->ModulePanel_1);
    QMenu*            menu = new QMenu(this);
    menu->addAction("New Collection", this, SLOT(onNewCollectionClicked()));
    ui->ModulePanel_1->addPanel("Collections", cm, menu);

    // **** MODULES RIGHT

    mHistogramModule = new LibraryHistogramModule(ui->ModulePanel_2);
    ui->ModulePanel_2->addPanel("Histogram", mHistogramModule);

    // Keywording (editing keywords module)
    mKeywording = new TaggingModule(ui->ModulePanel_2);
    ui->ModulePanel_2->addPanel("Keywords", mKeywording);

    // **** MODULE
    // Keyword list module
    FixedTreeView*   trvwKeywords = new FixedTreeView(ui->ModulePanel_2);
    SqlKeywordModel* keywordModel = new SqlKeywordModel(this);
    trvwKeywords->setModel(keywordModel);
    trvwKeywords->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->ModulePanel_2->addPanel("Keyword List", trvwKeywords);

    mMetaDataModule = new MetaDataModule(ui->ModulePanel_2);
    ui->ModulePanel_2->addPanel("Meta Data", mMetaDataModule);

    onShowGrid();

    mPhotoWorkUnit = PhotoWorkUnit::instance();

    // open up the last path location
    long long pathid =
        settings.value(SETTINGS_LIBRARY_FILES_PATHITEM).toLongLong();

    qDebug() << "Expanding";
    QModelIndex index = mPathModel->index(pathid);
    do
    {
        qDebug() << "+" << ((PathItem*)index.internalPointer())->path;
        mTrvwFiles->expand(index);
        index = index.parent();
    }
    while (index.isValid());

    emit photoSourceChanged(PhotoModel::SourceFiles, pathid);
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

    QModelIndex index = mTrvwFiles->currentIndex();

    if (index.isValid())
    {
        QVariant  v    = mPathModel->data(index, SqlPathModel::Path);
        PathItem* item = v.value<PathItem*>();

        if (item != NULL)
            settings.setValue(SETTINGS_LIBRARY_FILES_PATHITEM, item->id);
    }
    delete ui;
}

QRect Library::lightGap()
{
    QPoint pos = ui->mClvPhotos->mapToGlobal(QPoint(0, 0));
    QRect  gap = QRect(pos, ui->mClvPhotos->size());

    return gap;
}

bool Library::canSelectionChange()
{
    //    qDebug() << qApp->focusWidget()->objectName();
    return ui->mClvPhotos->hasFocus() || ui->mLoupeScrollView->hasFocus();
}

bool Library::canSelectUpDown()
{
    return ui->mClvPhotos->isVisible();
}

int Library::tilesPerRowOrCol()
{
    return ui->mClvPhotos->tilesPerColRow();
}

void Library::setSelectionModel(QItemSelectionModel* selectionModel)
{
    ui->mClvPhotos->setSelectionModel(selectionModel);
    connect(selectionModel, &QItemSelectionModel::selectionChanged,
        this, &Library::onPhotoSelectionChanged);
    connect(selectionModel, &QItemSelectionModel::currentChanged,
        this, &Library::onCurrentPhotoChanged);
}

void Library::onFilesClicked(const QModelIndex& index)
{
    // TODO: get the path model and get the file to query and show only those images in the view
    QVariant  v    = mPathModel->data(index, SqlPathModel::Path);
    PathItem* item = v.value<PathItem*>();

    // TODO: clear the filter
    // reset the photo model to the root of this item
    emit photoSourceChanged(PhotoModel::SourceFiles, item->id);
}

void Library::customContextMenu(const QPoint& /*pos*/)
{
    //QModelIndex index = ui->mClvPhotos->posToModelIndex(pos);
    // check if there is a single selection or a list.
    //    Photo info = mPhotoModel->data(index,TileView::PhotoRole).value<Photo>();

    // TODO: FIXME: fix popup
    //    QMenu *m = ui->menuPhoto;
    //    m->popup(ui->mClvPhotos->mapToGlobal(pos));
    //    m->exec();
}

void Library::onZoomLevelChanged(int zoomLevel)
{
    LoupeView::ZoomMode zoom[LoupeView::ZoomLast] = {
        LoupeView::ZoomFit, LoupeView::Zoom25, LoupeView::Zoom50,
        LoupeView::Zoom100, LoupeView::Zoom150, LoupeView::Zoom200,
        LoupeView::Zoom300, LoupeView::Zoom400, LoupeView::Zomm800
    };

    ui->mLoupeView->setZoomMode(zoom[zoomLevel]);
}

void Library::onThumbSizeChanged(int newValue)
{
    int w = ui->mClvPhotos->width();

    qDebug() << "Newvalue=" << newValue;

    int newMin = w / (newValue + 1);

    if (newMin < 50)
        newMin = 50;

    ui->mClvPhotos->setMinimumCellWidth(newMin);
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
    photos.append(mPhotoModel->data(index,
        TileView::TileView::ImageRole).value<Photo>());

    mKeywording->setPhotos(photos);
    mMetaDataModule->setPhotos(photos);
}

void Library::onCurrentPhotoChanged(const QModelIndex& current,
    const QModelIndex& /*previous*/)
{
    if (!current.isValid())
    {
        qDebug() << "No current photo selected";
        return;
    }
    Photo photo =
        mPhotoModel->data(current,
            TileView::TileView::ImageRole).value<Photo>();

    mCurrentPhoto = photo;
    mHistogramModule->setPhoto(photo);

    if (ui->mLoupeScrollView->isVisible())
        onShowLoupe();
}

void Library::onPathModelRowsAdded(const QModelIndex& /*parent*/,
    int /*start*/,
    int /*end*/)
{
    mPathModel->reload();
}

void Library::onPathModelRowsRemoved(const QModelIndex& /*parent*/,
    int /*start*/,
    int /*end*/)
{
    mPathModel->reload();
}

void Library::onTileDoubleClicked(const QModelIndex& /*index*/)
{
    onShowLoupe();
}

void Library::onCycleLoupeInfo()
{
    ui->mLoupeView->cycleInfoMode();
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
    ui->StackedWidget_1->setCurrentWidget(ui->mClvPhotos);
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

    if (order == Qt::AscendingOrder) {
        mPhotoModel->sort(0, Qt::DescendingOrder);
        ui->pbSortOrder->setText("↓");
    }
    else {
        mPhotoModel->sort(0, Qt::AscendingOrder);
        ui->pbSortOrder->setText("↑");
    }
}

void Library::onNewCollectionClicked()
{
    qDebug() << "create new collection";
}
}
