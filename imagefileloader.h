#ifndef IMAGEFILELOADER_H
#define IMAGEFILELOADER_H

#include <QObject>
#include <QModelIndex>
#include <QString>
#include <QImage>
#include <QRunnable>

#include "engine/colortransform.h"
#include "external/rawspeed/RawSpeed/RawSpeed-API.h"

using namespace RawSpeed;

class ImageFileLoader : public QObject, public QRunnable
{
    Q_OBJECT

    public:

        explicit ImageFileLoader(const QVariant &ref, const QString& path);
        ~ImageFileLoader();
        void run();

    signals:

        void dataReady(const QVariant& ref, const QImage& image);
        void error(QString error);

    private:

        QVariant       mRef;
        QString        mPath;
        ColorTransform mColorTransformRGBWorking;
        cmsHTRANSFORM  mHRawTransform;

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
        void mmultm(float* A, float* B, float* out);

        //        void vmultm(float* V, float* M, float* out);
        void normalize(float* M);
        void convertXyz65sRGB(float* src, uint8_t* dst, size_t size);
};

class Metadata
{
    static CameraMetaData* mMetaData;

    public:

        static CameraMetaData* metaData();
};

#endif // IMAGEFILELOADER_H
