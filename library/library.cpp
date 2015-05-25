#include "library.h"
#include "ui_library.h"

#include "library/imagedbtile.h"
#include "library/modules/keywordingmodule.h"

// models
#include "sqlkeywordmodel.h"

// widgets
#include "widgets/fixedtreeview.h"

Library::Library(PhotoModel * const model, QWidget *parent) :
    Module(parent),
    ui(new Ui::Library)
{
    mPhotoModel = model;

    ui->setupUi(this);

    ImageDbTile * tile = new ImageDbTile(ui->mClvPhotos);
    // connect(tile,&ImageDbTile::rotateLeftClicked, this, &Library::rotateLeftClicked);
    // connect(tile,&ImageDbTile::rotateRightClicked, this, &Library::rotateRightClicked);
    // connect(tile,&ImageDbTile::ratingClicked,this,&Library::ratingClicked);

    ui->mClvPhotos->setTileFlyweight(tile);
    ui->mClvPhotos->setMinimumCellWidth(150);
    ui->mClvPhotos->setMaximumCellWidth(200);
    ui->mClvPhotos->setCheckBoxMode(false);

    ui->mClvPhotos->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->mClvPhotos,&TileView::customContextMenuRequested,this,&Library::customContextMenu);

    // These models are auto deleted by the QObject hierarchy
    ui->mClvPhotos->setModel(mPhotoModel);

    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea_2->setWidgetResizable(true);

    // **** MODULE
    // Files module
    FixedTreeView * trvwFiles = new FixedTreeView(ui->ModulePanel_1);
    mPathModel = new SqlPathModel(this);
    trvwFiles->setModel(mPathModel);
    trvwFiles->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->ModulePanel_1->addPanel("Folders",trvwFiles);
    connect(trvwFiles,&FixedTreeView::clicked,this,&Library::onFilesClicked);

    // **** MODULE
    // Keywording (editing keywords module)
    mKeywording = new KeywordingModule(ui->ModulePanel_2);
    connect(ui->mClvPhotos,&TileView::selectionChanged,this,&Library::onPhotoSelectionChanged);
    ui->ModulePanel_2->addPanel("Keywords",mKeywording);

    // **** MODULE
    // Keyword list module
    FixedTreeView* trvwKeywords = new FixedTreeView(ui->ModulePanel_2);
    SqlKeywordModel* keywordModel = new SqlKeywordModel(this);
    trvwKeywords->setModel(keywordModel);
    trvwKeywords->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->ModulePanel_2->addPanel("Keyword List",trvwKeywords);

    mPhotoWorkUnit = PhotoWorkUnit::instance();
}

Library::~Library()
{
    delete ui;
}

QRect Library::lightGap()
{
    QPoint pos = ui->mClvPhotos->mapToGlobal(QPoint(0,0));
    QRect gap = QRect(pos,ui->mClvPhotos->size());
    return gap;
}

void Library::onFilesClicked(const QModelIndex &index)
{
    // TODO: get the path model and get the file to query and show only those images in the view
    QVariant v = mPathModel->data(index,SqlPathModel::Path);
    PathItem * item = v.value<PathItem*>();
    // reset the photo model to the root of this item
    emit photoSourceChanged(PhotoModel::SourceFiles,item->id);
}


void Library::customContextMenu(const QPoint &pos)
{
    QModelIndex index = ui->mClvPhotos->posToModelIndex(pos);
    // check if there is a single selection or a list.
    Photo* info = mPhotoModel->data(index,TileView::PhotoRole).value<Photo*>();

    // TODO: FIXME: fix popup
    //    QMenu *m = ui->menuPhoto;
    //    m->popup(ui->mClvPhotos->mapToGlobal(pos));
    //    m->exec();
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
void Library::onPhotoSelectionChanged()
{
    QList<QModelIndex> list= ui->mClvPhotos->selection();
    QList<Photo*> photos;
    QModelIndex index;
    foreach(index, list)
    {
        photos.append(mPhotoModel->data(index,TileView::PhotoRole).value<Photo*>());
    }
    mKeywording->setPhotos(photos);
    emit photoSelectionChanged(photos);
}
