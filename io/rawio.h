#ifndef PHOTOSTAGE_RAWIO_H
#define PHOTOSTAGE_RAWIO_H

#include <QString>
#include <QByteArray>

#include "image.h"
#include "import/exivfacade.h"
#include "external/rawspeed/RawSpeed/RawSpeed-API.h"

namespace PhotoStage
{
class RawIO
{
    public:

        RawIO();

        Image fromFile(const QString& filename);
        Image fromFile(const QByteArray& memFile, const PhotoStage::ExifInfo& ex_info);

    private:

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
        void getMatrix(float* in, float* out);
};

class Metadata
{
    static RawSpeed::CameraMetaData* mMetaData;

    public:

        static RawSpeed::CameraMetaData* metaData();
};
}
#endif // PHOTOSTAGE_RAWIO_H
