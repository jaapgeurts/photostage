#include <QDebug>
#include <setjmp.h>

#include "engine/colortransform.h"
#include "jpeglib.h"
#include "jpegio.h"
#include "image.h"
#include "external/iccjpeg/iccjpeg.h"
#include "utils.h"

namespace PhotoStage
{
void my_error_exit (j_common_ptr cinfo)
{
    qDebug() << "jpeg decode error";

    char jpegLastErrorMsg[JMSG_LENGTH_MAX];
    /* Create the message */
    ( *( cinfo->err->format_message ))( cinfo, jpegLastErrorMsg );

    /* Throw the exception */
    throw std::runtime_error( jpegLastErrorMsg );
}

Image JpegIO::fromFile(const QString& filename, const ExifInfo& ex_info)
{
    QFile f(filename);

    if (!f.open(QFile::ReadOnly))
    {
        qDebug() << "Can't open file" << filename;
        return Image();
    }

    QByteArray a = f.readAll();
    return fromFile(a, ex_info);
}

Image JpegIO::fromFile(const QByteArray& memFile, const ExifInfo& ex_info)
{
    QByteArray                    iccProfile;
    Image                         outImage;

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr         jerr;

    // setup error handling so that we will jump to our own code
    cinfo.err       = jpeg_std_error(&jerr);
    jerr.error_exit = my_error_exit;

    try
    {
        // setup the structs
        jpeg_create_decompress(&cinfo);
        jpeg_mem_src(&cinfo, (const unsigned char*)memFile.constData(), memFile.length());

        setup_read_icc_profile(&cinfo);

        // read the header
        jpeg_read_header(&cinfo, TRUE);

        jpeg_start_decompress(&cinfo);

        int      width      = cinfo.output_width;
        int      height     = cinfo.output_height;
        int      pixel_size = cinfo.output_components;
        uint8_t* buffer     = new uint8_t[width * height * pixel_size];
        uint8_t* line[1];

        //qDebug() << "reading jpeg" << width << "," << height << "," << pixel_size;

        cinfo.out_color_space = JCS_EXT_BGR;

        while (cinfo.output_scanline < height)
        {
            line[0] = &buffer[cinfo.output_scanline * pixel_size * width];
            jpeg_read_scanlines(&cinfo, line, 1);
        }

        // Convert to Image format and
        Image::Rotation r = Image::DontRotate;

        switch (ex_info.rotation)
        {
            case ExifInfo::Rotate90CW:
                r = Image::Rotate90CW;
                break;

            case ExifInfo::Rotate90CCW:
                r = Image::Rotate90CCW;
                break;
        }
        outImage = Image(width, height, pixel_size, buffer, r);

        // now read the ICC profile if there is any.
        uint8_t* icc_profile_buf;
        uint32_t buflen;

        if (read_icc_profile(&cinfo, &icc_profile_buf, &buflen))
        {
            qDebug() << "Found profile in JPEG";
            // There was a profile. read it into a buffer and pass it to QByteArray
            iccProfile = QByteArray((const char*)icc_profile_buf, buflen);
            free(icc_profile_buf);
        }
        jpeg_destroy_decompress(&cinfo);
    }
    catch (std::runtime_error& e)
    {
        qDebug() << "JPEG decode error" << e.what();

        jpeg_destroy_decompress(&cinfo);
        return outImage;
    }

    // Change the picture's color profile to Melissa32
    ColorTransform toWorking;

    if (!iccProfile.isEmpty())
    {
        // use the embedded JPeg profile
        toWorking = ColorTransform::getTransform(iccProfile, WORKING_COLOR_SPACE,
                ColorTransform::FORMAT_RGB48_PLANAR, ColorTransform::FORMAT_RGB48_PLANAR);
        //  mPhoto.exifInfo().profileName = toWorking.profileName();
    }
    else
    {
        // Assume default JPEG images are in sRGB format.
        toWorking = ColorTransform::getTransform("sRGB-Melissa-RGB32",
                "sRGB", WORKING_COLOR_SPACE, ColorTransform::FORMAT_RGB48_PLANAR, ColorTransform::FORMAT_RGB48_PLANAR);
        //  mPhoto.exifInfo().profileName = "sRGB (Assumed)";
    }

    return toWorking.transformImage(outImage);
}

bool JpegIO::saveToFile(const Image& image, const QString& filename)
{
    qDebug() << "bool JpegIO::saveToFile(const Image& image, const QString& filename) NOT IMPLEMENTED";
    return false;
}

JpegIO::JpegIO()
{
}
}