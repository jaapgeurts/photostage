#include <QDir>
#include <QDebug>
#include <QStandardPaths>

#include "previewcache.h"

namespace PhotoStage
{
PreviewCache* PreviewCache::mGlobalCache = NULL;

PreviewCache* PreviewCache::globalCache()
{
    if (mGlobalCache == NULL)
        mGlobalCache = new PreviewCache(QStandardPaths::writableLocation(
                    QStandardPaths::CacheLocation) + QDir::separator() + "previews",
                NULL);
    return mGlobalCache;
}

PreviewCache::PreviewCache(const QString& location, QObject* parent) :
    QObject(parent),
    mBaseDir(location),
    mHash(QCryptographicHash::Md5)
{
}

void PreviewCache::remove(const QString& key)
{
    std::pair<QString, QString> pair = dirFromKey(key);
    QString                     dir  = pair.first;
    QString                     hash = pair.second;

    QString                     fileName = dir + QDir::separator() + hash;


    if (!QFile::remove(fileName))
        qDebug() << "Can't remove preview cache file";
}

QImage PreviewCache::get(const QString& key)
{
    // Fetch the image from disk.
    std::pair<QString, QString> pair = dirFromKey(key);
    QString                     dir  = pair.first;
    QString                     hash = pair.second;

    QString                     fileName = dir + QDir::separator() + hash;


    if (QFileInfo::exists(fileName))
        return QImage(fileName);
    else
        return QImage();
}

void PreviewCache::put(const QString& key, const QImage& image)
{
    std::pair<QString, QString> pair = dirFromKey(key);
    QString                     dir  = pair.first;
    QString                     hash = pair.second;

    QString                     filePath = dir + QDir::separator() + hash;
    QDir                        d(dir);


    d.mkpath(dir);
    qDebug() << "Storing image at" << filePath;
    image.save(filePath, "jpg", 80);
}

std::pair<QString, QString> PreviewCache::dirFromKey(const QString& key)
{
    mHash.reset();
    mHash.addData((const char*)key.toUtf8().data());
    QString hash(mHash.result().toHex());
    QString f1, f2;
    f1 = hash.mid(0, 2);
    f2 = hash.mid(2, 2);
    QString dir = mBaseDir + QDir::separator() + f1 + QDir::separator() +
        f2;
    return std::make_pair(dir, hash);
}
}
