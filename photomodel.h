#ifndef QPHOTOMODEL_H
#define QPHOTOMODEL_H

#include <QAbstractListModel>
#include <QImage>
#include <QHash>

#include "photo.h"
#include "previewcache.h"
#include "threadqueue.h"
#include "workunits/photoworkunit.h"

namespace PhotoStage
{
class PhotoModel : public QAbstractListModel
{
    Q_OBJECT

    friend class Photo;

    public:

        enum SourceType
        {
            SourceFiles      = 1,
            SourceCollection = 2
        };

        // constructors
        PhotoModel(QObject* parent = 0);
        ~PhotoModel();

        // getters / setters
        int rowCount(const QModelIndex& parent) const;
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
        void refreshData(const QList<Photo>&);
        void addData(const QList<long long>& idList);

    public slots:

        void onPhotoSourceChanged(SourceType source, long long pathId);
        void onVisibleTilesChanged(const QModelIndex& start,
            const QModelIndex& end);

    private slots:

        void previewGenerated(Photo photo, const QImage& image);
        void imageTranslated(Photo photo, const QImage& image);
        void previewLoaded(Photo photo, const QImage& image);

    private:

        PhotoWorkUnit*                mWorkUnit;
        ThreadQueue*                  mThreadQueue;
        // The mPhotoInfoList is the main container for the Photo Objects.
        QList<Photo>                  mPhotoList;
        QHash<long long, QModelIndex> mPhotoIndexMap;
        QHash<long long, uint32_t>    mRunningThreads;

        void loadImage(Photo& photo);
        void convertImage(Photo& photo);
};
}

#endif // QPHOTOMODEL_H
