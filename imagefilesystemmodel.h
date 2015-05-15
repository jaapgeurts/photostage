#ifndef IMAGEFILESYSTEMMODEL_H
#define IMAGEFILESYSTEMMODEL_H

#include <QFileSystemModel>
#include <QHash>
#include <QImage>
#include <QModelIndex>
#include <QThreadPool>


class ImageFileSystemModel : public QFileSystemModel
{
    Q_OBJECT

public:

    ImageFileSystemModel(QObject* parent = 0);
    ~ImageFileSystemModel();

    QVariant data(const QModelIndex &index, int role) const;

    void clearCache();

public slots:

    void imageLoaded(const QModelIndex &index, const QImage& pixmap);
    void loaderError(QString error);

private:
    QHash<QModelIndex,QImage> *mPixmapCache;
    QThreadPool *mThreadPool;
};

#endif // IMAGEFILESYSTEMMODEL_H
