#include <QCoreApplication>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

#include "rawio.h"

#include "external/rawspeed/src/librawspeed/metadata/CameraMetadataException.h"
#include "external/rawspeed/src/librawspeed/decoders/RawDecoderException.h"
#include "import/exivfacade.h"
#include "engine/stage0raw.h"

template<typename T>
static inline long max(T x, T y)
{
    return x > y ? x : y;
}

template<typename T>
static inline long min(T x, T y)
{
    return x < y ? x : y;
}

template<typename T>
static inline long clip(T v, T x, T y)
{
    return max(x, min(v, y));
}

#define SRC(x, y) (src[x] * src[y])

using namespace rawspeed;

namespace PhotoStage
{
CameraMetaData* Metadata::mMetaData = NULL;
CameraMetaData* Metadata::metaData()
{
    if (mMetaData == NULL)
    {
        try
        {
            QString filePath = QCoreApplication::applicationDirPath() + "/../Resources/cameras.xml";
            mMetaData = new CameraMetaData(filePath.toLocal8Bit().data());
        }
        catch (CameraMetadataException& e)
        {
            // Reading metadata failed. e.what() will contain error message.
            qDebug() << "Error reading metadata" << e.what();
        }
    }
    return mMetaData;
}

RawIO::RawIO()
{
}

RawIO::RawIO(const QByteArray& memFile, const QSharedPointer<DevelopRawParameters>& params, const QString& cameraModel)
{
    initFromFile(memFile, params, cameraModel);
}

const Image& RawIO::image() const
{
    return mImage;
}

const QByteArray RawIO::colorProfile() const
{
    return mProfile;
}

/**
 * @brief ImageFileLoader::compute_inverse computes the inverse of a matrix
 * @param src the input matrix
 * @param dst (out parameter) the result
 * @return true, if successful, false if the inverse doesn't exist
 */
bool RawIO::compute_inverse(const float src[9], float dst[9]) const
{
    bool result = false;

    memset(dst, 0, 9 * sizeof(float));

    // See http://en.wikipedia.org/wiki/Invertible_matrix for algorithm
    float determinant;
    float A, B, C, D, E, F, G, H, I;
    A = SRC(4, 8) - SRC(5, 7);
    B = -(SRC(3, 8) - SRC(5, 6));
    C = SRC(3, 7) - SRC(4, 6);
    D = -(SRC(1, 8) - SRC(2, 7));
    E = SRC(0, 8) - SRC(2, 6);
    F = -(SRC(0, 7) - SRC(1, 6));
    G = SRC(1, 5) - SRC(2, 4);
    H = -(SRC(0, 5) - SRC(2, 3));
    I = SRC(0, 4) - SRC(1, 3);

    determinant = src[0] * A + src[1] * B + src[2] * C;

    if (determinant == 0)
    {
        result = false;
    }
    else
    {
        float t = 1.0 / determinant;
        dst[0] = t * A;
        dst[1] = t * D;
        dst[2] = t * G;
        dst[3] = t * B;
        dst[4] = t * E;
        dst[5] = t * H;
        dst[6] = t * C;
        dst[7] = t * F;
        dst[8] = t * I;
        result = true;
    }
    return result;
}

void RawIO::dump_matrix(const QString& name, float m[9]) const
{
    QString space = QString(name.length(), ' ');

    qDebug() << space << "┌";
    qDebug() << space << "│" << m[0] << " " << m[1] << " " << m[2];
    qDebug() << name << "│" << m[3] << " " << m[4] << " " << m[5];
    qDebug() << space << "│" << m[6] << " " << m[7] << " " << m[8];
    qDebug() << space << "└";
}

int RawIO::compute_cct(float R, float G, float B) const
{
    // see here for more details
    // http://dsp.stackexchange.com/questions/8949/how-do-i-calculate-the-color-temperature-of-the-light-source-illuminating-an-ima
    float X, Y, Z;

    X = (-0.14282) * (R)+(1.54924) * (G) + (-0.95641) * (B);
    Y = (-0.32466) * (R)+(1.57837) * (G) + (-0.73191) * (B);
    Z = (-0.68202) * (R)+(0.77073) * (G)+(0.56332) * (B);
    float x, y;
    x = X / (X + Y + Z);
    y = Y / (X + Y + Z);
    float n;
    n = (x - 0.3320) / (0.1858 - y);
    int   CCT = 449 * n * n * n + 3525 * n * n + 6823.3 * n + 5520.33;

    return CCT;
}

void RawIO::mmultm(float* A, float* B, float* out) const
{
    memset(out, 0, 9 * sizeof(float));

    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            out[r * 3 + c] += A[r * 3] * B[c + 3 * c];

}

void RawIO::normalize(float* M) const
{
    float sum;

    for (int i = 0; i < 3; i++)
    {
        sum = 0;

        for (int j = 0; j < 3; j++)
            sum += M[i * 3 + j];

        for (int j = 0; j < 3; j++)
            M[i * 3 + j] /= sum;
    }
}

void RawIO::prepareMatrix(float* in, float* out) const
{
    for (int i = 0; i < 9; i++)
        in[i] /= 10000;

    normalize(in);
    compute_inverse(in, out);
}

bool RawIO::readMatrix(const QString& model, float* mat) const
{
    QFile f(QCoreApplication::applicationDirPath() + "/../Resources/camera_color_matrix.json");

    f.open(QFile::ReadOnly);
    QByteArray    a = f.readAll();
    f.close();
    QJsonDocument document   = QJsonDocument::fromJson(a);
    QJsonArray    cameraList = document.array();

    for (int i = 0; i < cameraList.size(); ++i)
    {
        QJsonObject camera       = cameraList[i].toObject();
        QJsonArray  modelAliases = camera.value("model").toArray();

        for (int j = 0; j < modelAliases.size(); j++)
        {
            QString modelName = modelAliases.at(j).toString();

            if (modelName.compare(model, Qt::CaseInsensitive))
            {
                QJsonArray matrix = camera.value("color_matrix").toArray();

                if (matrix.size() != 9)
                {
                    qDebug() << "Currently only 3x3 color matrices are supported (= 9 values)";
                    return false;
                }

                for (int j = 0; j < matrix.size(); j++)
                    mat[j] = (float)matrix[j].toDouble();

                return true;
            }
        }
    }

    return false;
}

Image RawIO::initFromFile(const QByteArray& memFile,
    const QSharedPointer<DevelopRawParameters>& params, const QString& cameraModel)
{
    // TODO: check memory leak here.
    std::unique_ptr<const Buffer> map(new Buffer((const uchar8*)memFile.constData(), memFile.length()));

    std::unique_ptr<RawDecoder> decoder;
    try
    {
        RawParser parser(map.get());
        decoder = parser.getDecoder();
    }
    catch (const RawDecoderException& e)
    {
        qDebug() << "(1) Can't acquire decoder for";
        return mImage;
    }

    if (decoder == NULL)
    {
        qDebug() << "(2) Can't acquire decoder for";
        return mImage;
    }
    decoder->failOnUnknown = 0;
    decoder->checkSupport(Metadata::metaData());

    try
    {
        decoder->decodeRaw();
        decoder->decodeMetaData(Metadata::metaData());
    }
    catch (const RawDecoderException& e)
    {
        qDebug() << "Can't decode image";
        qDebug() << "Exception:" << e.what();
        return mImage;
    }

    RawImage raw = decoder->mRaw;
   // raw->scaleBlackWhite();
    int      bl = raw->blackLevel;
    int      wp = raw->whitePoint;

    int          components_per_pixel = raw->getCpp();
    int          bytes_per_pixel      = raw->getBpp();
    RawImageType type                 = raw->getDataType();
    bool         is_cfa               = raw->isCFA;

    if (is_cfa && components_per_pixel == 1 && type == TYPE_USHORT16 && bytes_per_pixel == 2)
    {
        ColorFilterArray cfa        = raw->cfa;
        uint32_t         cfa_layout = cfa.getDcrawFilter();

        uint16_t*        rawdata        = (uint16_t*)raw->getData(0, 0);
        int              pitch_in_bytes = raw->pitch;

        pitch_in_bytes /= 2;

        uint16_t* data = new uint16_t[raw->dim.x * raw->dim.y];

        // copy pixels to my buffer
        for (int y = 0; y < raw->dim.y; y++)
        {
            uint16_t* row = &data[y * raw->dim.x];

            // planar is not a problem now since raw only contains one plane
            for (int x = 0; x < raw->dim.x; x++)
                row[x] = rawdata[y * pitch_in_bytes + x];
        }

        // get the camera color conversion matrix
        float mat[9];
        // set the src matrix to the identity matrix
        float srcMatrix[9];
        bool  cameraFound;

        cameraFound = readMatrix(cameraModel, srcMatrix);

        if (!cameraFound)
        {
            qDebug() << "Camera model" << cameraModel << "not found. Using identity matrix.";
            float identity[9] = { 10000, 0, 0, 0, 10000, 0, 0, 0, 10000 };
            prepareMatrix(identity, mat);
        }
        else
        {
            prepareMatrix(srcMatrix, mat);
        }

        // qDebug () << "Using WB factors" << wbr << "," << wbg << "," << wbb;

        Stage0Raw pb;
        // pass the develop settings here.

        pb.setDevelopParams(params);

        pb.setDomain(bl, wp);
        pb.setColorConversion(mat);
        pb.setInput(data, raw->dim.x, raw->dim.y);
        pb.setCFAStart(cfa_layout);
        //  pb.setInterpolationAlgorithm(PipelineBuilder::Bilinear);

        pb.prepare();

        mImage = pb.execute();
        delete data;
    }
    return mImage;
}
}
