#ifndef IMAGEFILELOADER_H
#define IMAGEFILELOADER_H

#include <QObject>
#include <QModelIndex>
#include <QString>
#include <QQueue>
#include <QRunnable>
#include <QThread>
#include <QMutex>

#include "image.h"
#include "engine/colortransform.h"
#include "external/rawspeed/RawSpeed/RawSpeed-API.h"

using namespace RawSpeed;

struct Job
{
    Job()
    {
    }

    Job(const QVariant& ref, const QString& path);
    QVariant ref;
    QString path;
};

class ImageFileLoader : public QObject, public QRunnable
{
    Q_OBJECT

    public:

        static ImageFileLoader* getLoader();

        explicit ImageFileLoader();
        ~ImageFileLoader();

        void addJob(const QVariant& ref, const QString& path);

        void run();

    signals:

        void dataReady(const QVariant& ref, const QImage& image);
        void error(const QVariant& ref, QString error);

    private:

        static ImageFileLoader* mLoader;

        QMutex                  mMutexJobs;
        QQueue<Job>             mJobs;
        QThread                 mThread;

        QImage loadRaw(const QString& path);

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
        Job hasMore(QQueue<Job>& queue);
        QImage rawThumb(const QString& path);
        void getMatrix(float *in, float*out);
};

class Metadata
{
    static CameraMetaData* mMetaData;

    public:

        static CameraMetaData* metaData();
};

#endif // IMAGEFILELOADER_H
