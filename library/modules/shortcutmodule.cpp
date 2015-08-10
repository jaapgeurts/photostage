#include <QVBoxLayout>

#include "shortcutmodule.h"

namespace PhotoStage
{
ShortcutModule::ShortcutModule(QWidget* parent) :
    LibraryModule(parent)
{
    mLvWorkList = new Widgets::FixedListView(this);
    mLvWorkList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mLvWorkList->setDragEnabled(false);
    mLvWorkList->setAcceptDrops(true);
    mLvWorkList->setDropIndicatorShown(true);

    connect(mLvWorkList, &QListView::clicked, this, &ShortcutModule::onWorkListClicked);

    mWorkModel = new CollectionModel(this, CollectionDAO::WorkSource);
    mLvWorkList->setModel(mWorkModel);

    mLvImportList = new Widgets::FixedListView(this);
    mLvImportList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mLvImportList->setDragEnabled(false);
    mLvImportList->setAcceptDrops(false);

    connect(mLvImportList, &QListView::clicked, this, &ShortcutModule::onImportListClicked);

    mImportModel = new CollectionModel(this, CollectionDAO::ImportSource);
    mLvImportList->setModel(mImportModel);

    setLayout(new QVBoxLayout(this));
    layout()->addWidget(mLvWorkList);
    layout()->addWidget(mLvImportList);
    layout()->setContentsMargins(0, 0, 0, 0);
}

void ShortcutModule::sourceChanged(PhotoModel::SourceType source)
{
    if (source == PhotoModel::SourceCollectionUser || source == PhotoModel::SourceFiles)
    {
        mLvImportList->selectionModel()->clear();
        mLvWorkList->selectionModel()->clear();
    }
    else if (source == PhotoModel::SourceCollectionImport)
    {
        mLvWorkList->selectionModel()->clear();
    }
    else if (source == PhotoModel::SourceCollectionWork)
    {
        mLvImportList->selectionModel()->clear();
    }
}

void ShortcutModule::onWorkListClicked(const QModelIndex& index)
{
    // TODO: get the path model and get the file to query and show only those images in the view
    CollectionItem* item = mWorkModel->data(index, CollectionModel::CollectionRole).value<CollectionItem*>();

    emit            workCollectionSelected(item->id);
}

void ShortcutModule::onImportListClicked(const QModelIndex& index)
{
    // TODO: get the path model and get the file to query and show only those images in the view
    CollectionItem* item = mImportModel->data(index, CollectionModel::CollectionRole).value<CollectionItem*>();

    emit            importCollectionSelected(item->id);
}
}
