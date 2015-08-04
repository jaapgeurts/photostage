#include <QFileSystemModel>
#include <QVBoxLayout>
#include <QEvent>
#include <QMimeData>
#include <QDragEnterEvent>

#include <QDebug>

#include "constants.h"
#include "dragdropinfo.h"
#include "collectionmodel.h"
#include "collectionmodule.h"

namespace PhotoStage
{
class DropHandler : public Widgets::DndHandler, public QObject
{
    public:

        DropHandler(CollectionModule* module, QObject* parent = 0);

        bool dragStart(const QModelIndex& index, QMimeData* mimeData, QPixmap& image, QPoint& hotspot);
        void dragEnter(QDragEnterEvent* event);
        void dragLeave(QDragLeaveEvent* event);
        void dragOver(QDragMoveEvent* event);
        void dragDrop(QDropEvent* event);

    private:

        CollectionModule* mModule;
};

DropHandler::DropHandler(CollectionModule* module, QObject* parent) :
    QObject(parent),
    mModule(module)
{
}

bool DropHandler::dragStart(const QModelIndex& index, QMimeData* mimeData, QPixmap& image, QPoint& hotspot)
{
    return false;
}

void DropHandler::dragEnter(QDragEnterEvent* event)
{
    event->acceptProposedAction();
}

void DropHandler::dragLeave(QDragLeaveEvent* event)
{
}

void DropHandler::dragOver(QDragMoveEvent* event)
{
    event->acceptProposedAction();
}

void DropHandler::dragDrop(QDropEvent* event)
{
    mModule->handleDrop(event);
}

CollectionModule::CollectionModule(QWidget* parent) : LibraryModule(parent)
{
    mTrvwCollections = new Widgets::FixedTreeView(this);
    mTrvwCollections->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mTrvwCollections->setAcceptDrops(true);
    mTrvwCollections->setDropIndicatorShown(true);
    mTrvwCollections->setDragDropMode(QAbstractItemView::DropOnly);
    mTrvwCollections->setDragEnabled(false);
    mTrvwCollections->setDndHandler(new DropHandler(this, this));

    mCollectionModel = new CollectionModel(this);
    mTrvwCollections->setModel(mCollectionModel);

    //    connect(mTrvwCollections, &FixedTreeView::clicked, this, &CollectionModule::onFilesClicked);
    //    connect(mCollectionModel, &CollectionModel::rowsInserted, this, &CollectionModule::onPathModelRowsAdded);
    //    connect(mCollectionModel, &CollectionModel::rowsRemoved, this, &CollectionModule::onPathModelRowsRemoved);

    setLayout(new QVBoxLayout(this));
    layout()->addWidget(mTrvwCollections);
    layout()->setContentsMargins(0, 0, 0, 0);
}

void CollectionModule::handleDrop(QDropEvent* event)
{
    DragDropInfo info(event->mimeData()->data(MIMETYPE_TILEVIEW_SELECTION));

    qDebug() << "drop handler()";

    if (info.sourceModel() == DragDropInfo::PhotoModel)
    {
        // get the directory the files were dropped on
        QPoint      pos   = event->pos();
        QModelIndex index = mTrvwCollections->indexAt(pos);

        if (index.isValid())
        {
            CollectionItem* item = mCollectionModel
                ->data(index, CollectionModel::CollectionRole).value<CollectionItem*>();
            qDebug() << "Dropped on" << item->name;
            event->acceptProposedAction();
        }
        else
        {
            event->ignore();
        }
    }
}
}
