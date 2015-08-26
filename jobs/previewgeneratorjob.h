#ifndef PHOTOSTAGE_PREVIEWGENERATORJOB_H
#define PHOTOSTAGE_PREVIEWGENERATORJOB_H

#include <QObject>
#include <QModelIndex>
#include <QString>

#include "photo.h"
#include "runnable.h"
#include "engine/colortransform.h"
#include "external/rawspeed/RawSpeed/RawSpeed-API.h"

namespace PhotoStage
{
class PreviewGeneratorJob : public QObject, public Runnable
{
    Q_OBJECT

    public:

        explicit PreviewGeneratorJob(const Photo& photo);
        ~PreviewGeneratorJob();

    public:

        // Runnable interface
        QVariant run();
        void finished(QVariant result);
        void error(const QString& error);
        void cancel();

    signals:

        void imageReady(Photo photo, const QImage& image);

    private:

        Photo mPhoto;

        /**
         * @brief ImageFileLoader::compute_inverse computes the inverse of a matrix
         * @param src the input matrix
         * @param dst (out parameter) the result
         * @return true, if successful, false if the inverse doesn't exist
         */
        bool compute_inverse(const float src[], float dst[]);
        void dump_matrix(const QString& name, float m[]);
        int compute_cct(float R, float G, float B);
        void mmultm(float* A, float* B, float* out);

        //        void vmultm(float* V, float* M, float* out);
        void normalize(float* M);
        QImage genThumb(const QString& path);
        QImage rawThumb(const QByteArray &memFile);
        void getMatrix(float* in, float* out);
};

class Metadata
{
    static RawSpeed::CameraMetaData* mMetaData;

    public:

        static RawSpeed::CameraMetaData* metaData();
};
}
#endif // PHOTOSTAGE_PREVIEWGENERATORJOB_H
