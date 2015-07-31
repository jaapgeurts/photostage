#include <QDirIterator>
#include <QThread>
#include <QDebug>

#include "database/photodao.h"
#include "imagefilesystemmodel.h"
#include "previewfileloader.h"
#include "widgets/tileview.h"
#include "utils.h"

namespace PhotoStage
{
ImageFileSystemModel::ImageFileSystemModel(QObject* parent) :
    QAbstractListModel(parent),
    mRootPath(),
    mFilters(),
    mIncludeSubdirs(true)
{
    mPreviewInfoCache = new QHash<QModelIndex, PreviewInfo>();
    mThreadPool       = new QThreadPool(this);

    // FIXME: add this as a list to the preferences
    // or provide an option to ignore extensions
    mFilters << "*.png" << "*.jpg" << "*.jpeg" << "*.cr2" << "*.crw" << "*.nef" << "*.dng";
}

ImageFileSystemModel::~ImageFileSystemModel()
{
    mPreviewInfoCache->clear();
    delete mPreviewInfoCache;
}

QVariant ImageFileSystemModel::data(const QModelIndex& index, int role) const
{
    PreviewInfo info;

    if (!index.isValid())
        return QVariant();

    const QString path = mFileList.at(index.row());

    switch (role)
    {
        case Qt::DisplayRole:
            return path;
            break;

        case TileView::TileView::ImageRole:

            if (mPreviewInfoCache->contains(index))
            {
                // return cached image
                info = mPreviewInfoCache->value(index);
            }
            else
            {
                info.filePath = path;
                // load image in background thread
                PreviewFileLoader* loader = new PreviewFileLoader(path, index);
                connect(loader, &PreviewFileLoader::dataReady, this, &ImageFileSystemModel::imageLoaded);
                mThreadPool->start(loader);

                // Insert the dummy image here so that the we know the loader thread has been started
                mPreviewInfoCache->insert(index, info);

                return QVariant();
            }
            return QVariant::fromValue<PreviewInfo>(info);

        default:
            return QVariant();
    }
}

void ImageFileSystemModel::clearCache()
{
    mPreviewInfoCache->clear();
}

void ImageFileSystemModel::imageLoaded(const QModelIndex& index, const QImage& pixmap)
{
    PreviewInfo info = mPreviewInfoCache->value(index);

    info.isInLibrary =  PhotoDAO::instance()->IsInLibrary(computeImageFileHash(info.filePath));
    info.image       = pixmap;
    mPreviewInfoCache->insert(index, info);
    QVector<int> roles;
    roles.append(TileView::TileView::ImageRole);
    emit         dataChanged(index, index, roles);
}

void ImageFileSystemModel::loaderError(QString error)
{
    qDebug() << "Can't load image: " << error;
}

int ImageFileSystemModel::rowCount(const QModelIndex& /*parent*/) const
{
    return mFileList.size();
}

QModelIndex ImageFileSystemModel::setRootPath(const QString& rootPath)
{
    beginResetModel();
    mRootPath = rootPath;
    mFileList.clear();
    populate();
    endResetModel();
    return QModelIndex();
}

QFileInfo ImageFileSystemModel::fileInfo(const QModelIndex& index)
{
    return QFileInfo(mFileList.at(index.row()));
}

void ImageFileSystemModel::setIncludeSubdirs(bool include)
{
    mIncludeSubdirs = include;
    beginResetModel();
    mFileList.clear();
    populate();
    endResetModel();
}

bool ImageFileSystemModel::includeSubdirs() const
{
    return mIncludeSubdirs;
}

void ImageFileSystemModel::populate()
{
    QDirIterator iter(mRootPath,
        mFilters,
        QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks,
        mIncludeSubdirs ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);

    while (iter.hasNext())
    {
        mFileList << iter.next();
    }
}
}
