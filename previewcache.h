#ifndef PREVIEWCACHE_H
#define PREVIEWCACHE_H

#include <QObject>
#include <QImage>
#include <QCryptographicHash>

class PreviewCache : public QObject
{
    Q_OBJECT

    public:

        explicit PreviewCache(const QString& baseDir, QObject* parent = 0);

        QImage get(const QString& key);
        void put(const QString& key, const QImage& image);

    public slots:

    private:

        QString            mBaseDir;
        QCryptographicHash mHash;
        std::pair<QString, QString> dirFromKey(const QString &key);
};

#endif // PREVIEWCACHE_H
