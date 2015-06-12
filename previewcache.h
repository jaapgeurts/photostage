#ifndef PREVIEWCACHE_H
#define PREVIEWCACHE_H

#include <QObject>
#include <QCryptographicHash>

#include "image.h"

class PreviewCache : public QObject
{
    Q_OBJECT

    public:

        explicit PreviewCache(const QString& baseDir, QObject* parent = 0);

        Image get(const QString& key);
        void put(const QString& key, const Image& image);

    public slots:

    private:

        QString            mBaseDir;
        QCryptographicHash mHash;
        std::pair<QString, QString> dirFromKey(const QString &key);
};

#endif // PREVIEWCACHE_H
