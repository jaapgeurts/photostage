#ifndef PHOTOSTAGE_PHOTOMODEL_H
#define PHOTOSTAGE_PHOTOMODEL_H

#include <QAbstractListModel>
#include <QItemSelection>
#include <QImage>
#include <QHash>
#include <QMimeData>

#include "photo.h"
#include "photoowner.h"
#include "threadqueue.h"
#include "database/databaseaccess.h"

namespace PhotoStage
{
class PhotoModel : public QAbstractListModel, public PhotoOwner
{
    Q_OBJECT

    friend class Photo;

    public:

        enum SourceType
        {
            SourceFiles = 1,
            SourceCollectionUser,
            SourceCollectionWork,
            SourceCollectionImport
        };

        // constructors
        PhotoModel(QObject* parent = 0);
        ~PhotoModel();

        // getters / setters
        int rowCount(const QModelIndex& parent = QModelIndex()) const;
        QVariant headerData(int, Qt::Orientation, int) const;
        QVariant data(const QModelIndex& index, int role) const;

        /*
           // these methods are for editing
           Qt::ItemFlags flags(const QModelIndex &index) const;
           bool setData(const QModelIndex &index, const QVariant &value, int role);

           // these methods are for updating(add/remove)
           bool insertRows(int row, int count, const QModelIndex &parent);
           bool removeRows(int row, int count, const QModelIndex &parent);
         */
        void refreshData(const QList<Photo>& list);
        void addData(const QList<long long>& idList);

        QList<Photo> toList() const;

        QList<Photo> toList(const QItemSelection& selection) const;

        bool removeRows(int row, int count, const QModelIndex& parent);
        long long rootId();

        Qt::DropActions supportedDragActions() const;
        Qt::DropActions supportedDropActions() const;
        QStringList mimeTypes() const;
        QMimeData* mimeData(const QModelIndexList& indexes) const;
        bool canDropMimeData(const QMimeData*, Qt::DropAction, int, int, const QModelIndex&) const;
        bool dropMimeData(const QMimeData*, Qt::DropAction, int, int, const QModelIndex&);

        SourceType rootSource() const;

    public slots:

        void setRootSourceId(SourceType source, long long id);
        void onVisibleTilesChanged(const QModelIndex& start, const QModelIndex& end);

    private slots:

        // from DAO
        void onPhotosAdded(long long pathid, const QList<long long>& idList);
        void onPhotosDeleted(const QList<Photo>& photos);
        void onPhotosDeletedFromCollection(long long collectionid, const QList<Photo>& photos);
        void onPhotosChanged(const QList<Photo>& photos);

        void onPreviewTranslated(Photo photo, const QImage& image);
        void onOriginalTranslated(Photo photo, const QImage& image);

        void onPreviewLoaded(Photo photo, const QImage& image);
        void onOriginalLoaded(Photo photo, const Image& image);

        void onExifUpdated(Photo photo);
        void onSaveParams(Photo photo, const DevelopRawParameters& params);

    private:

        PhotoDAO*                     mWorkUnit;
        ThreadQueue*                  mPreviewThreadQueue;
        ThreadQueue*                  mOriginalThreadQueue;
        // The mPhotoInfoList is the main container for the Photo Objects.
        QList<Photo>                  mPhotoList;
        QHash<long long, QModelIndex> mPhotoIndexMap;
        QHash<long long, uint32_t>    mPreviewLoadThreads;
        QHash<long long, uint32_t>    mOriginalLoadThreads;
        long long                     mRootId;
        SourceType                    mPhotoSource;

        void loadPreview(Photo& photo);
        void loadOriginal(Photo& photo);
        void convertImage(Photo& photo);
        void convertOriginal(Photo& photo);
};
}

#endif // PHOTOSTAGE_PHOTOMODEL_H
