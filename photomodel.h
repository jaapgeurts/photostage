#ifndef QPHOTOMODEL_H
#define QPHOTOMODEL_H

#include <QAbstractListModel>
#include <QtSql>
#include <QHash>

#include "workunits/photoworkunit.h"

#include "photo.h"


class PhotoModel : public QAbstractListModel
{
public:

    enum SourceType { SourceFiles = 1, SourceCollection = 2 };

    PhotoModel(QObject* parent = 0);
    ~PhotoModel();

    int rowCount(const QModelIndex &parent) const;
    QVariant headerData(int, Qt::Orientation, int) const;
    QVariant data(const QModelIndex &index, int role) const;

    /*
    // these methods are for editing
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    // these methods are for updating(add/remove)
    bool insertRows(int row, int count, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent);
    */

    void refreshData(const QList<Photo*> &);
    void addData(const QList<long long> &idList);

public slots:
    void onReloadPhotos(SourceType source, long long id );

private slots:
    void imageLoaded(const QModelIndex &index, const QImage &image);

private:
    PhotoWorkUnit * mWorkUnit;
    // The mPhotoInfoList is the main container for the Photo Objects.
    // Delete is required on it's contents
    QList<Photo*> mPhotoInfoList;
    QHash<QModelIndex,Photo*> *mPhotoInfoCache;
    QThreadPool *mThreadPool;

};

#endif // QPHOTOMODEL_H
