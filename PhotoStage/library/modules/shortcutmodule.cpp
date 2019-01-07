#include <QVBoxLayout>

#include "shortcutmodule.h"
#include "database/collectionitem.h"

namespace PhotoStage
{
ShortcutModule::ShortcutModule(QWidget* parent) :
    LibraryModule(parent)
{
    mLvShortcutList = new Widgets::FixedListView(this);
    mLvShortcutList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mLvShortcutList->setDragEnabled(false);
    mLvShortcutList->setAcceptDrops(true);
    mLvShortcutList->setDropIndicatorShown(true);

    connect(mLvShortcutList, &QListView::clicked, this, &ShortcutModule::onListClicked);

    mCollectionModel = new CollectionModel(this, CollectionDAO::WorkSource | CollectionDAO::ImportSource);
    mLvShortcutList->setModel(mCollectionModel);

    setLayout(new QVBoxLayout(this));
    layout()->addWidget(mLvShortcutList);
    layout()->setContentsMargins(0, 0, 0, 0);
}

void ShortcutModule::sourceChanged(PhotoModel::SourceType source)
{
    if (source != PhotoModel::SourceCollectionWork && source != PhotoModel::SourceCollectionImport)
    {
        mLvShortcutList->selectionModel()->clear();
    }
}

void ShortcutModule::onListClicked(const QModelIndex& index)
{
    // TODO: get the path model and get the file to query and show only those images in the view
    CollectionItem* item = mCollectionModel->data(index, CollectionModel::CollectionRole).value<CollectionItem*>();

    if (item->source == CollectionDAO::WorkSource)
        emit workCollectionSelected(item->id);
    else if (item->source == CollectionDAO::ImportSource)
        emit importCollectionSelected(item->id);
}
}
