#ifndef PREVIEWINFO_H
#define PREVIEWINFO_H

#include <QString>
#include <QImage>
#include <QMetaType>

class PreviewInfo
{
public:
    PreviewInfo() {};
    PreviewInfo(const QImage& img, const QString& path);

    QImage image;
    QString filePath;
};

Q_DECLARE_METATYPE(PreviewInfo)

#endif // PREVIEWINFO_H
