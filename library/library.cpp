#include <QMenu>

#include "library.h"
#include "ui_library.h"

// models
#include "sqlkeywordmodel.h"

#define SETTINGS_SPLITTER_LIBRARY_SIZES "librarymodule/splitter_main"
#define SETTINGS_LIBRARY_FILES_PATHITEM "librarymodule/files/pathitem"

namespace PhotoStage
{
Library::Library(PhotoModel* const model, QWidget* parent) :
    Module(parent),
    ui(new Ui::Library),
    mFontAccessFoundIcons(QFont("Accessibility Foundicons", 15))
{
    mPhotoModel = model;

    ui->setupUi(this);

    QSettings  settings;
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

    ui->mClvPhotos->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->mClvPhotos,
        &TileView::customContextMenuRequested,
        this,
        &Library::customContextMenu);
    connect(ui->mClvPhotos,
        &TileView::doubleClickTile,
        this,
        &Library::onTileDoubleClicked);

    // These models are auto deleted by the QObject hierarchy
    ui->mClvPhotos->setModel(mPhotoModel);

    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea_2->setWidgetResizable(true);

    // **** MODULES LEFT

    // shortcuts module
    ShortcutModule* sm = new ShortcutModule(ui->ModulePanel_1);
    ui->ModulePanel_1->addPanel("Shortcuts", sm);

    // Files module
    mTrvwFiles = new FixedTreeView(ui->ModulePanel_1);
    mPathModel = new SqlPathModel(this);
    mTrvwFiles->setModel(mPathModel);
    mTrvwFiles->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->ModulePanel_1->addPanel("Folders", mTrvwFiles);
    connect(mTrvwFiles,
        &FixedTreeView::clicked,
        this,
        &Library::onFilesClicked);

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

    showGrid();

    mPhotoWorkUnit = PhotoWorkUnit::instance();

    // open up the last path location
    long long pathid =
        settings.value(SETTINGS_LIBRARY_FILES_PATHITEM).toLongLong();

    QModelIndex index = mPathModel->index(pathid);
    do
    {
        mTrvwFiles->expand(index);
        index = index.parent();
    }
    while (index.isValid());

    emit photoSourceChanged(PhotoModel::SourceFiles, pathid);
}

Library::~Library()
{
    QSettings    settings;
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
    return ui->mClvPhotos->tilesPerRowOrCol();
}

void Library::setSelectionModel(QItemSelectionModel* selectionModel)
{
    ui->mClvPhotos->setSelectionModel(selectionModel);
    connect(selectionModel,
        &QItemSelectionModel::selectionChanged,
        this,
        &Library::onPhotoSelectionChanged);
}

void Library::onFilesClicked(const QModelIndex& index)
{
    // TODO: get the path model and get the file to query and show only those images in the view
    QVariant  v    = mPathModel->data(index, SqlPathModel::Path);
    PathItem* item = v.value<PathItem*>();
    // reset the photo model to the root of this item
    emit      photoSourceChanged(PhotoModel::SourceFiles, item->id);
}

void Library::customContextMenu(const QPoint& pos)
{
    //QModelIndex index = ui->mClvPhotos->posToModelIndex(pos);
    // check if there is a single selection or a list.
    //    Photo info = mPhotoModel->data(index,TileView::PhotoRole).value<Photo>();

    // TODO: FIXME: fix popup
    //    QMenu *m = ui->menuPhoto;
    //    m->popup(ui->mClvPhotos->mapToGlobal(pos));
    //    m->exec();
}

void Library::thumbSizeChanged(int newValue)
{
    // todo: change the number of columns here.
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
        TileView::PhotoRole).value<Photo>());

    mKeywording->setPhotos(photos);
    mHistogramModule->setPhotos(photos);

    // TODO: just take the first one in the selection
    if (!photos.isEmpty())
    {
        mCurrentPhoto = photos.first();

        if (ui->mLoupeScrollView->isVisible())
        {
            showLoupe();
        }
    }

    emit photoSelectionChanged(photos);
}

void Library::onTileDoubleClicked(const QModelIndex& index)
{
    showLoupe();
}

void Library::showLoupe()
{
    if (!mCurrentPhoto.isNull())
    {
        ui->mLoupeView->setPhoto(mCurrentPhoto);
        ui->StackedWidget_1->setCurrentWidget(ui->mLoupeScrollView);
    }
}

void Library::showGrid()
{
    ui->StackedWidget_1->setCurrentWidget(ui->mClvPhotos);
}

void Library::onNewCollectionClicked()
{
    qDebug() << "create new collection";
}
}
