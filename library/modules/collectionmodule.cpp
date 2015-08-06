#include <QMenu>
#include <QFileSystemModel>
#include <QVBoxLayout>
#include <QInputDialog>

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

    connect(mTrvwCollections, &Widgets::FixedTreeView::clicked, this, &CollectionModule::onFilesClicked);
    //    connect(mCollectionModel, &CollectionModel::rowsInserted, this, &CollectionModule::onPathModelRowsAdded);
    //    connect(mCollectionModel, &CollectionModel::rowsRemoved, this, &CollectionModule::onPathModelRowsRemoved);

    setLayout(new QVBoxLayout(this));
    layout()->addWidget(mTrvwCollections);
    layout()->setContentsMargins(0, 0, 0, 0);
}

void CollectionModule::onFilesClicked(const QModelIndex& index)
{
    // TODO: get the path model and get the file to query and show only those images in the view
    CollectionItem* item = mCollectionModel->data(index, CollectionModel::CollectionRole).value<CollectionItem*>();

    emit            collectionSelected(item->id);
}

QMenu* CollectionModule::getMenu()
{
    QMenu* menu = new QMenu(this);

    menu->addAction("New Collection", this, SLOT(onNewCollectionClicked()));

    return menu;
}

void CollectionModule::onNewCollectionClicked()
{
    bool    ok;
    QString name = QInputDialog::getText(this, "New Collection", "Name", QLineEdit::Normal, QString(), &ok);

    if (ok && !name.isEmpty())
    {
        QModelIndex index = mTrvwCollections->currentIndex();

        if (!index.isValid())
        {
            CollectionDAO::instance()->addCollection(nullptr, name);
        }
        else
        {
            CollectionItem* item =
                mCollectionModel->data(index, CollectionModel::CollectionRole).value<CollectionItem*>();
            CollectionDAO::instance()->addCollection(Nullable<long long>(item->id), name);
        }
    }
}
}
