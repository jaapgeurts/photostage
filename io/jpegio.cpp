#include <QDebug>
#include <setjmp.h>

#include "jpeglib.h"
#include "jpegio.h"
#include "external/iccjpeg/iccjpeg.h"
#include "utils.h"

namespace PhotoStage
{
struct my_error_mgr
{
    struct jpeg_error_mgr pub;  /* "public" fields */

    jmp_buf setjmp_buffer;      /* for return to caller */
};

typedef struct my_error_mgr* my_error_ptr;

void my_error_exit (j_common_ptr cinfo)
{
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    my_error_ptr myerr = (my_error_ptr)cinfo->err;

    qDebug() << "jpeg decode error";

    /* Always display the message. */
    /* We could postpone this until after returning, if we chose. */
    (*cinfo->err->output_message)(cinfo);

    /* Return control to the setjmp point */
    longjmp(myerr->setjmp_buffer, 1);
}

QImage JpegIO::readFile(const QByteArray& memFile, QByteArray& iccProfile)
{
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr           jerr;

    // setup error handling so that we will jump to our own code
    cinfo.err           = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer))
    {
        qDebug() << "return from jpeg decode error";

        jpeg_destroy_decompress(&cinfo);
        return QImage();
    }

    // setup the structs
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, (const unsigned char*)memFile.constData(), memFile.length());

    setup_read_icc_profile(&cinfo);

    // read the header
    jpeg_read_header(&cinfo, TRUE);

    jpeg_start_decompress(&cinfo);

    int      width      = cinfo.output_width;
    int      height     = cinfo.output_height;
    int      pixel_size = cinfo.output_components + 1;
    uint8_t* buffer     = new uint8_t[width * height * pixel_size];
    uint8_t* line[1];

    //qDebug() << "reading jpeg" << width << "," << height << "," << pixel_size;

    // set the output to BGRX so that it aligns to 32bit (expected format for QImage)
    cinfo.out_color_space = JCS_EXT_BGRX;

    while (cinfo.output_scanline < height)
    {
        line[0] = &buffer[cinfo.output_scanline * pixel_size * width];
        jpeg_read_scanlines(&cinfo, line, 1);
    }

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

    return QImage(buffer, width, height, pixel_size * width,
               QImage::Format_RGB32, (QImageCleanupFunction)deleteArray<uint8_t>, buffer);
}

JpegIO::JpegIO()
{
}
}
