#ifndef PHOTOSTAGE_RAWIO_H
#define PHOTOSTAGE_RAWIO_H

#include <QString>
#include <QByteArray>

#include "image.h"
#include "engine/developrawparameters.h"
#include "external/rawspeed/RawSpeed/RawSpeed-API.h"

namespace PhotoStage
{
class RawIO
{
    public:

        RawIO();
        RawIO(const QByteArray &memFile, const DevelopRawParameters &params, const QString &cameraModel);

        const Image& image() const;
        const QByteArray colorProfile() const;

    private:

        Image initFromFile(const QByteArray& memFile, const DevelopRawParameters &params, const QString &cameraModel);

        /**
         * @brief ImageFileLoader::compute_inverse computes the inverse of a matrix
         * @param src the input matrix
         * @param dst (out parameter) the result
         * @return true, if successful, false if the inverse doesn't exist
         */
        bool compute_inverse(const float src[], float dst[]) const;
        void dump_matrix(const QString& name, float m[]) const;
        int compute_cct(float R, float G, float B) const;
        void mmultm(float* A, float* B, float* out) const;

        //        void vmultm(float* V, float* M, float* out);
        void normalize(float* M) const;
        bool readMatrix(const QString& model, float* mat) const;
        void prepareMatrix(float* in, float* out) const;

        Image      mImage;
        QByteArray mProfile;
};

class Metadata
{
    static RawSpeed::CameraMetaData* mMetaData;

    public:

        static RawSpeed::CameraMetaData* metaData();
};
}
#endif // PHOTOSTAGE_RAWIO_H
