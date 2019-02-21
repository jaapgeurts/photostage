#ifndef PREVIEWFILELOADER_H
#define PREVIEWFILELOADER_H

#include <QImage>
#include <QModelIndex>
#include <QObject>
#include <QRunnable>
#include <QString>

namespace PhotoStage {
class PreviewFileLoader : public QObject, public QRunnable
{
  Q_OBJECT

public:
  explicit PreviewFileLoader(const QString& path, const QModelIndex& index);
  ~PreviewFileLoader();

  void run();

signals:

  void dataReady(const QModelIndex& index, const QImage& pixmap);
  void error(QString error);

private:
  QModelIndex mModelIndex;
  QString     mPath;
};
} // namespace PhotoStage

#endif // PREVIEWFILELOADER_H