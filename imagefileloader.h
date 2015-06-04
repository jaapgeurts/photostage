#ifndef IMAGEFILELOADER_H
#define IMAGEFILELOADER_H

#include <QObject>
#include <QModelIndex>
#include <QString>
#include <QImage>
#include <QRunnable>

#include "external/rawspeed/RawSpeed/RawSpeed-API.h"

using namespace RawSpeed;

class ImageFileLoader : public QObject, public QRunnable
{
    Q_OBJECT


public:
    explicit ImageFileLoader(const QString& path, const QModelIndex& index);
    ~ImageFileLoader();

    void run();

signals:

    void dataReady(const QModelIndex& index, const QImage& pixmap);
    void error(QString error);


private:
    QModelIndex mModelIndex;
    QString mPath;
    QImage loadRaw();

    /**
     * @brief ImageFileLoader::compute_inverse computes the inverse of a matrix
     * @param src the input matrix
     * @param dst (out parameter) the result
     * @return true, if successful, false if the inverse doesn't exist
     */
    bool compute_inverse(const float src[], float dst[]);
    void dump_matrix(const QString &name, float m[]);
    int compute_cct(float R, float G, float B);
    void mmultm(float *A, float *B, float *out);
    void vmultm(float *V, float *M, float *out);
    void normalize(float *M);
};

class Metadata
{
    static CameraMetaData *mMetaData;
public:

    static CameraMetaData *metaData();
};

#endif // IMAGEFILELOADER_H
