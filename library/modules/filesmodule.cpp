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
    mTrvwFiles = new FixedTreeView(this);
    mTrvwFiles->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mTrvwFiles->setAcceptDrops(true);
    mTrvwFiles->installEventFilter(this);

    mPathModel = new SqlPathModel(this);
    mTrvwFiles->setModel(mPathModel);

    connect(mTrvwFiles, &FixedTreeView::clicked, this, &FilesModule::onFilesClicked);
    connect(mPathModel, &SqlPathModel::rowsInserted, this, &FilesModule::onPathModelRowsAdded);
    connect(mPathModel, &SqlPathModel::rowsRemoved, this, &FilesModule::onPathModelRowsRemoved);

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
        PathItem* item = mPathModel->data(index, SqlPathModel::Path).value<PathItem*>();
        settings.setValue(SETTINGS_LIBRARY_FILES_PATHITEM, item->id);
    }
}

void FilesModule::reload()
{
    mPathModel->reload();
}

void FilesModule::onFilesClicked(const QModelIndex& index)
{
    // TODO: get the path model and get the file to query and show only those images in the view
    PathItem* item = mPathModel->data(index, SqlPathModel::Path).value<PathItem*>();

    emit      pathSelected(item->id);
}

void FilesModule::onPathModelRowsAdded(const QModelIndex& /*parent*/, int /*start*/, int /*end*/)
{
    mPathModel->reload();
}

void FilesModule::onPathModelRowsRemoved(const QModelIndex& /*parent*/, int /*start*/, int /*end*/)
{
    mPathModel->reload();
}

bool FilesModule::eventFilter(QObject* obj, QEvent* event)
{
    if (obj != mTrvwFiles)
        return false;

    bool ret = false;

    switch (event->type())
    {
        case QEvent::DragEnter:
        {
            QDragEnterEvent* evt = dynamic_cast<QDragEnterEvent*>(event);
            evt->acceptProposedAction();
            ret = true;
        }
        break;

        case QEvent::DragLeave:
            break;

        case QEvent::DragMove:
            break;

        case QEvent::Drop:
            ret = handleDrop(dynamic_cast<QDropEvent*>(event));
            break;

        default:
            ret = QObject::eventFilter(obj, event);
    }
    return ret;
}

bool FilesModule::handleDrop(QDropEvent* event)
{
    DragDropInfo info(event->mimeData()->data(MIMETYPE_TILEVIEW_SELECTION));

    if (info.sourceModel() == DragDropInfo::PathModel)
    {
        // TODO: do something with the drop. => move the files, if not in the same dir
    }
    event->acceptProposedAction();
    return true;
}
}
