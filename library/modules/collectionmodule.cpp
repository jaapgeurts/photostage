#include <QFileSystemModel>
#include <QVBoxLayout>

#include <QDebug>

#include "constants.h"
#include "dragdropinfo.h"
#include "collectionmodel.h"
#include "collectionmodule.h"

namespace PhotoStage
{
CollectionModule::CollectionModule(QWidget* parent) : LibraryModule(parent)
{
    mTrvwCollections = new Widgets::FixedTreeView(this);
    mTrvwCollections->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mTrvwCollections->setDragEnabled(false);
    mTrvwCollections->setAcceptDrops(true);
    mTrvwCollections->setDropIndicatorShown(true);

    mCollectionModel = new CollectionModel(this);
    mTrvwCollections->setModel(mCollectionModel);

    //    connect(mTrvwCollections, &FixedTreeView::clicked, this, &CollectionModule::onFilesClicked);
    //    connect(mCollectionModel, &CollectionModel::rowsInserted, this, &CollectionModule::onPathModelRowsAdded);
    //    connect(mCollectionModel, &CollectionModel::rowsRemoved, this, &CollectionModule::onPathModelRowsRemoved);

    setLayout(new QVBoxLayout(this));
    layout()->addWidget(mTrvwCollections);
    layout()->setContentsMargins(0, 0, 0, 0);
}
}
