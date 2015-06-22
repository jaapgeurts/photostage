#include <QDir>
#include <QDebug>

#include "previewcache.h"

namespace PhotoStage
{
PreviewCache::PreviewCache(const QString& baseDir, QObject* parent) :
    QObject(parent),
    mBaseDir(baseDir + QDir::separator() + "previews"),
    mHash(QCryptographicHash::Md5)
{
}

QImage PreviewCache::get(const QString& key)
{
    // Fetch the image from disk.
    std::pair<QString, QString> pair = dirFromKey(key);
    QString                     dir  = pair.first;
    QString                     hash = pair.second;

    QString                     fileName = dir + QDir::separator() + hash;

    return QImage(fileName);
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