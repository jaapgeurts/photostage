#ifndef IMAGEFILELOADER_H
#define IMAGEFILELOADER_H

#include <QObject>
#include <QModelIndex>
#include <QString>
#include <QImage>
#include <QRunnable>

class ImageFileLoader : public QObject, public QRunnable
{
    Q_OBJECT

public:
    explicit ImageFileLoader(const QString& path, const QModelIndex& index);
    ~ImageFileLoader();

    void run();

signals:

    void dataReady(const QModelIndex& index, const QImage& pixmap);
    void error(QString error);


private:
    QModelIndex mModelIndex;
    QString mPath;
};

#endif // IMAGEFILELOADER_H
