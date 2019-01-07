#include <QFileSystemModel>
#include <QVBoxLayout>
#include <QInputDialog>

#include <QDebug>

#include "constants.h"
#include "database/databaseaccess.h"
#include "database/collectionitem.h"
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

    connect(mTrvwCollections, &Widgets::FixedTreeView::clicked, this, &CollectionModule::onFilesClicked);

    setLayout(new QVBoxLayout(this));
    layout()->addWidget(mTrvwCollections);
    layout()->setContentsMargins(0, 0, 0, 0);
}

void CollectionModule::sourceChanged(PhotoModel::SourceType source)
{
    if (source != PhotoModel::SourceCollectionUser)
    {
        mTrvwCollections->selectionModel()->clear();
    }
}

void CollectionModule::onNewCollection()
{
    bool    ok;
    QString name = QInputDialog::getText(this, "New Collection", "Name", QLineEdit::Normal, QString(), &ok);

    if (ok && !name.isEmpty())
    {
        QModelIndex index = mTrvwCollections->currentIndex();

        if (!index.isValid())
        {
            DatabaseAccess::collectionDao()->addCollection(nullptr, name);
        }
        else
        {
            CollectionItem* item =
                mCollectionModel->data(index, CollectionModel::CollectionRole).value<CollectionItem*>();
            DatabaseAccess::collectionDao()->addCollection(Nullable<long long>(item->id), name);
        }
    }
}

void CollectionModule::onFilesClicked(const QModelIndex& index)
{
    // TODO: get the path model and get the file to query and show only those images in the view
    CollectionItem* item = mCollectionModel->data(index, CollectionModel::CollectionRole).value<CollectionItem*>();

    emit            collectionSelected(item->id);
}
}
