#ifndef PREVIEWFILELOADER_H
#define PREVIEWFILELOADER_H

#include <QObject>
#include <QModelIndex>
#include <QString>
#include <QImage>
#include <QRunnable>

namespace PhotoStage
{
    class PreviewFileLoader : public QObject, public QRunnable
    {
        Q_OBJECT

        public:

            explicit PreviewFileLoader(const QString& path,
                const QModelIndex& index);
            ~PreviewFileLoader();

            void run();

        signals:

            void dataReady(const QModelIndex& index, const QImage& pixmap);
            void error(QString error);

        private:

            QModelIndex mModelIndex;
            QString     mPath;
    };
}

#endif // PREVIEWFILELOADER_H