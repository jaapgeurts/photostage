#include <QSettings>
#include <QVBoxLayout>
#include <QEvent>
#include <QDropEvent>
#include <QMimeData>

#include <QDebug>

#include "filesmodule.h"
#include "dragdropinfo.h"
#include "constants.h"

namespace PhotoStage
{
FilesModule::FilesModule(QWidget* parent) : LibraryModule(parent)
{
    mTrvwFiles = new Widgets::FixedTreeView(this);
    mTrvwFiles->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mTrvwFiles->setAcceptDrops(true);
    mTrvwFiles->installEventFilter(this);
    mTrvwFiles->setDropIndicatorShown(true);
    mTrvwFiles->setDragDropMode(QAbstractItemView::DropOnly);

    mPathModel = new PathModel(this);
    mTrvwFiles->setModel(mPathModel);

    connect(mTrvwFiles, &Widgets::FixedTreeView::clicked, this, &FilesModule::onFilesClicked);

    setLayout(new QVBoxLayout(this));
    layout()->addWidget(mTrvwFiles);
    layout()->setContentsMargins(0, 0, 0, 0);

    // open up the last path location
    QSettings   settings;
    long long   pathid = settings.value(SETTINGS_LIBRARY_FILES_PATHITEM).toLongLong();

    QModelIndex index = mPathModel->index(pathid);

    while (index.isValid())
    {
        mTrvwFiles->expand(index);
        index = index.parent();
    }

    emit pathSelected(pathid);
}

FilesModule::~FilesModule()
{
    QSettings   settings;
    QModelIndex index = mTrvwFiles->currentIndex();

    if (index.isValid())
    {
        PathItem* item = mPathModel->data(index, PathModel::PathRole).value<PathItem*>();
        settings.setValue(SETTINGS_LIBRARY_FILES_PATHITEM, item->id);
    }
}

void FilesModule::onFilesClicked(const QModelIndex& index)
{
    // TODO: get the path model and get the file to query and show only those images in the view
    PathItem* item = mPathModel->data(index, PathModel::PathRole).value<PathItem*>();

    emit      pathSelected(item->id);
}
}
