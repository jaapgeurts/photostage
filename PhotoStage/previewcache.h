#ifndef PREVIEWCACHE_H
#define PREVIEWCACHE_H

#include <QCryptographicHash>
#include <QImage>
#include <QObject>

namespace PhotoStage {
class PreviewCache : public QObject
{
  Q_OBJECT

public:
  static PreviewCache* globalCache();

  explicit PreviewCache(const QString& location, QObject* parent = 0);

  QImage get(const QString& key);
  void   put(const QString& key, const QImage& image);
  void   remove(const QString& key);
  void   setImageFormat(QString format);

public slots:

private:
  static PreviewCache* mGlobalCache;

  QString                     mBaseDir;
  QCryptographicHash          mHash;
  QString                     mFormat;
  std::pair<QString, QString> dirFromKey(const QString& key);
};
} // namespace PhotoStage
#endif // PREVIEWCACHE_H
