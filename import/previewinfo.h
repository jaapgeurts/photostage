#ifndef PHOTOSTAGE_PREVIEWINFO_H
#define PHOTOSTAGE_PREVIEWINFO_H

#include <QString>
#include <QImage>
#include <QMetaType>

namespace PhotoStage
{
class PreviewInfo
{
    public:

        PreviewInfo();
        PreviewInfo(const QImage& img, const QString& path);

        QImage  image;
        QString filePath;
        bool    isInLibrary;
};
}
Q_DECLARE_METATYPE(PhotoStage::PreviewInfo)

#endif // PHOTOSTAGE_PREVIEWINFO_H
