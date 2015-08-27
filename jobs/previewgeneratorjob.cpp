#include <QDebug>
#include <QFileInfo>
#include <QThread>

#include <lcms2.h>

#include "constants.h"
#include "previewgeneratorjob.h"
#include "import/exivfacade.h"
#include "engine/colortransform.h"
#include "engine/pipelinebuilder.h"
#include "io/jpegio.h"

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

using namespace RawSpeed;

namespace PhotoStage
{
CameraMetaData* Metadata::mMetaData = NULL;
CameraMetaData* Metadata::metaData()
{
    if (mMetaData == NULL)
    {
        try
        {
            mMetaData = new CameraMetaData(
                "/Users/jaapg/Development/PhotoStage/PhotoStage/external/rawspeed/data/cameras.xml");
        }
        catch (CameraMetadataException& e)
        {
            // Reading metadata failed. e.what() will contain error message.
            qDebug() << "Error reading metadata" << e.what();
        }
    }
    return mMetaData;
}

PreviewGeneratorJob::PreviewGeneratorJob(const Photo& photo) :
    QObject(NULL),
    mPhoto(photo)
{
    setName("PreviewGeneratorJob");
}

PreviewGeneratorJob::~PreviewGeneratorJob()
{
}

QVariant PreviewGeneratorJob::run()
{
    QImage image = genThumb(mPhoto.srcImagePath());

    return image;
}

void PreviewGeneratorJob::finished(QVariant result)
{
    QImage image = result.value<QImage>();
    emit   imageReady(mPhoto, image);
}

void PreviewGeneratorJob::error(const QString& error)
{
    qDebug() << "Error during image load or gen thumb" << error;
}

void PreviewGeneratorJob::cancel()
{
    mPhoto.setIsDownloading(false);
}

QImage PreviewGeneratorJob::genThumb(const QString& path)
{
    // TODO: catch errors and emit error(QString)
    QImage image;

    // load the file into a buffer first.
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug("Can't open file");
        return image;
    }
    QByteArray memFile = file.readAll();
    file.close();

    QString suffix = QFileInfo(path).suffix().toUpper();

    // TODOL
    if (suffix == "NEF" || suffix == "CR2" || suffix == "CRW")
    {
        qDebug() << "Load raw" << path;

        image = rawThumb(memFile);
    }
    else
    {
        // Read using JPEG library
        //qDebug() << "Load jpg" << path;
        QByteArray iccProfile;
        // use QImage loading. this ignores embedded profiles
        //QImage     pixmap = QImage::fromData(memFile);
        QImage pixmap = JpegIO::readFile(memFile, iccProfile);

        // rotate the image if necessary
        ExivFacade* ex = ExivFacade::createExivReader();

        if (!ex->openData(memFile))
        {
            qDebug() << "Error loading exif data from image";
            return image;
        }
        ExifInfo ex_info = ex->data();
        delete(ex);

        QTransform m;

        switch (ex_info.rotation)
        {
            case ExifInfo::Rotate90CCW:
                m.rotate(-90);
                pixmap = pixmap.transformed(m);
                break;

            case ExifInfo::Rotate90CW:
                m.rotate(90);
                pixmap = pixmap.transformed(m);
                break;

            default:
                // qDebug() << "Unimplemented rotation value";
                break;
        }

        if (!pixmap.isNull())
        {
            // Assume default JPEG images are in sRGB format.
            // Convert the picture to MelissaRGB space
            ColorTransform::Format fmt;
            ColorTransform         toWorking;

            if (pixmap.format() == QImage::Format_RGB32)
            {
                fmt = ColorTransform::FORMAT_RGB32;

                if (!iccProfile.isEmpty())
                {
                    // use the embedded JPeg profile
                    toWorking = ColorTransform::getTransform(iccProfile, WORKING_COLOR_SPACE,
                            fmt, ColorTransform::FORMAT_RGB32);
                    mPhoto.exifInfo().profileName = toWorking.profileName();
                }
                else
                {
                    toWorking = ColorTransform::getTransform("sRGB-Melissa-RGB32",
                            "sRGB", WORKING_COLOR_SPACE, fmt, ColorTransform::FORMAT_RGB32);
                    mPhoto.exifInfo().profileName = "sRGB (Assumed)";
                }
            }
            else if (pixmap.format() == QImage::Format_Grayscale8)
            {
                //                qDebug() << "alpha" << pixmap.hasAlphaChannel();
                //                qDebug() << "bpp" << pixmap.depth();
                //                qDebug() << "Channels" << pixmap.bitPlaneCount();
                fmt       = ColorTransform::FORMAT_GRAYSCALE8;
                toWorking = ColorTransform::getTransform("sRGB-Melissa-Gray8",
                        "sRGB", WORKING_COLOR_SPACE, fmt, ColorTransform::FORMAT_RGB32);
                mPhoto.exifInfo().profileName = "sRGB (Assumed)";
            }
            else
            {
                qDebug() << "**** QImage format not supported";
                return image;
            }

            image = toWorking.transformQImage(pixmap);
        }
    }

    if (image.isNull())
    {
        qDebug() << "image loading failed";
    }
    return image;
}

/**
 * @brief ImageFileLoader::compute_inverse computes the inverse of a matrix
 * @param src the input matrix
 * @param dst (out parameter) the result
 * @return true, if successful, false if the inverse doesn't exist
 */
bool PreviewGeneratorJob::compute_inverse(const float src[9], float dst[9])
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

void PreviewGeneratorJob::dump_matrix(const QString& name, float m[9])
{
    QString space = QString(name.length(), ' ');

    qDebug() << space << "┌";
    qDebug() << space << "│" << m[0] << " " << m[1] << " " << m[2];
    qDebug() << name << "│" << m[3] << " " << m[4] << " " << m[5];
    qDebug() << space << "│" << m[6] << " " << m[7] << " " << m[8];
    qDebug() << space << "└";
}

int PreviewGeneratorJob::compute_cct(float R, float G, float B)
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

void PreviewGeneratorJob::mmultm(float* A, float* B, float* out)
{
    out[0] = A[0] * B[0] + A[1] * B[3] + A[2] * B[6];
    out[1] = A[0] * B[1] + A[1] * B[4] + A[2] * B[7];
    out[2] = A[0] * B[2] + A[1] * B[5] + A[2] * B[8];

    out[3] = A[3] * B[0] + A[4] * B[3] + A[5] * B[6];
    out[4] = A[3] * B[1] + A[4] * B[4] + A[5] * B[7];
    out[5] = A[3] * B[2] + A[4] * B[5] + A[5] * B[8];

    out[6] = A[6] * B[0] + A[7] * B[3] + A[8] * B[6];
    out[7] = A[6] * B[1] + A[7] * B[4] + A[8] * B[7];
    out[8] = A[6] * B[2] + A[7] * B[5] + A[8] * B[8];
}

//void ImageFileLoader::vmultm(float* V,float* M,float* out)
//{
//}

void PreviewGeneratorJob::normalize(float* M)
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

void PreviewGeneratorJob::getMatrix(float* in, float* out)
{
    for (int i = 0; i < 9; i++)
        in[i] /= 10000;

    normalize(in);
    compute_inverse(in, out);
}

QImage PreviewGeneratorJob::rawThumb(const QByteArray& memFile)
{
    QImage      image;

    ExivFacade* ex = ExivFacade::createExivReader();

    if (!ex->openData(memFile))
    {
        qDebug() << "Error loading exif data from image";
        return image;
    }
    ExifInfo ex_info = ex->data();
    delete(ex);

    //FileReader reader(strdup(path.toLocal8Bit().data()));
    QSharedPointer<FileMap> map;
    try
    {
        // qDebug() << "loadRaw() opening" << reader.Filename();
        //        map = reader.readFile();
        map = QSharedPointer<FileMap>(new FileMap((uchar8*)memFile.constData(), (uint32)memFile.length()));
    }
    catch (const FileIOException& e)
    {
        qDebug() << "Error reading raw" << e.what();
        return image;
    }

    // remove raw from here
    if (map == NULL)
    {
        qDebug() << "Can't acquire map for raw file";
        return image;
    }

    RawDecoder* decoder = NULL;
    try
    {
        RawParser parser(map.data());
        decoder = parser.getDecoder();
    }
    catch (const RawDecoderException& e)
    {
        qDebug() << "(1) Can't acquire decoder for";
        return image;
    }

    if (decoder == NULL)
    {
        qDebug() << "(2) Can't acquire decoder for";
        return image;
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
        return image;
    }

    RawImage     raw = decoder->mRaw;
    //raw->scaleBlackWhite();
    int          bl = raw->blackLevel;
    int          wp = raw->whitePoint;

    int          components_per_pixel = raw->getCpp();
    int          bytes_per_pixel      = raw->getBpp();
    RawImageType type                 = raw->getDataType();
    bool         is_cfa               = raw->isCFA;

    if (is_cfa && components_per_pixel == 1 && type == TYPE_USHORT16 &&
        bytes_per_pixel == 2)
    {
        ColorFilterArray cfa        = raw->cfa;
        uint32_t         cfa_layout = cfa.getDcrawFilter();

        if (cfa_layout == 0x94949494)
            qDebug() << "RGGB";

        if (cfa_layout == 0x49494949)
            qDebug() << "GBRG";

        if (cfa_layout == 0x61616161)
            qDebug() << "GRBG";

        if (cfa_layout == 0x16161616)
            qDebug() << "BGGR";

        uint16_t vert, horz;

        if (cfa_layout == 0x94949494)
            vert = horz = 0;
        else if (cfa_layout == 0x49494949)
        {
            horz = 0;
            vert = 1;
        }

        uint16_t* rawdata        = (uint16_t*)raw->getData(0, 0);
        int       width          = raw->dim.x;
        int       height         = raw->dim.y;
        int       pitch_in_bytes = raw->pitch;

        width          -= 2;     // cut of the borders off for simplicity.
        height         -= 2;
        pitch_in_bytes /= 2;

        Halide::Image<uint16_t> rawh(raw->dim.x, raw->dim.y, "Raw Image");
        uint16_t*               data = rawh.data();

        for (int y = 0; y < raw->dim.y; y++)
        {
            uint16_t* row = &data[y * raw->dim.x];

            // planar is not a problem now since raw only contains one plane
            for (int x = 0; x < raw->dim.x; x++)
                row[x] = rawdata[y * pitch_in_bytes + x];
        }

        // apply white balance
        float wbr = ex_info.rgbCoeffients[0];
        float wbg = ex_info.rgbCoeffients[1];
        float wbb = ex_info.rgbCoeffients[2];

        float canon300d[9] =
        { 8197, -2000, -1118, -6714, 14335, 2592, -2536, 3178, 8266 };
        float canon350d[9] =
        { 6018, -617, -965, -8645, 15881, 2975, -1530, 1719, 7642 };
        float powershots30[9] =
        { 10566, -3652, -1129, -6552, 14662, 2006, -2197, 2581, 7670 };
        float canon5DMarkII[9] =
        { 4716, 603, -830, -7798, 15474, 2480, -1496, 1937, 6651 };
        float eos1100d[9] =
        { 6444, -904, -893, -4563, 12308, 2535, -903, 2016, 6728 };
        float nikonD300[9] =
        { 9030, -1992, -715, -8465, 16302, 2255, -2689, 3217, 8069 };
        float powershots110[9] =
        { 8039, -2643, -654, -3783, 11230, 2930, -206, 690, 4194 };
        float identity[9] =
        { 10000, 0, 0, 0, 10000, 0, 0, 0, 10000 };

        float mat[9];

        if (*ex_info.model == "Canon EOS 350D DIGITAL")
            getMatrix(canon350d, mat);
        else if (*ex_info.model == "Canon EOS 300D DIGITAL" ||
            *ex_info.model == "Canon EOS DIGITAL REBEL")
            getMatrix(canon300d, mat);
        else if (*ex_info.model == "Canon PowerShot S30")
            getMatrix(powershots30, mat);
        else if (*ex_info.model == "Canon EOS 5D Mark II")
            getMatrix(canon5DMarkII, mat);
        else if (*ex_info.model == "Canon EOS REBEL T3")
            getMatrix(eos1100d, mat);
        else if (*ex_info.model == "NIKON D300")
            getMatrix(nikonD300, mat);
        else if (*ex_info.model == "Canon PowerShot S110")
            getMatrix(powershots110, mat);
        else
        {
            qDebug() << "Color conversion matrix for camera model" <<
                *ex_info.model << "unavailable.";
            getMatrix(identity, mat);
        }

        qDebug () << "Using WB factors" << wbr << "," << wbg << "," << wbb;

        PipelineBuilder pb;
        pb.prepare();

        pb.setWhiteBalance(wbr, wbg, wbb);
        pb.setDomain(bl, wp);
        pb.setColorConversion(mat);
        pb.setInput(rawh);
        pb.setCFAStart(cfa_layout);

        switch (ex_info.rotation)
        {
            case ExifInfo::Rotate90CCW:
                pb.setRotation(-1);
                break;

            case ExifInfo::Rotate90CW:
                pb.setRotation(1);
                break;

            default:
                qDebug() << "Unimplemented rotation value";
        }

        image = pb.execute(width, height);
    }
    return image;
}
}
