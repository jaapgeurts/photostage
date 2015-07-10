#ifndef PREVIEWCACHE_H
#define PREVIEWCACHE_H

#include <QObject>
#include <QImage>
#include <QCryptographicHash>

namespace PhotoStage
{
class PreviewCache : public QObject
{
    Q_OBJECT

    public:

        static PreviewCache *globalCache();

        explicit PreviewCache(const QString& location, QObject* parent = 0);

        QImage get(const QString& key);
        void put(const QString& key, const QImage& image);    
        void remove(const QString &key);

public slots:

    private:

        static PreviewCache* mGlobalCache;

        QString              mBaseDir;
        QCryptographicHash   mHash;
        std::pair<QString, QString> dirFromKey(const QString& key);
};
}
#endif // PREVIEWCACHE_H
