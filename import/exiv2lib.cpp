#include <QDebug>

#include "exiv2lib.h"
#include "exiv2/image.hpp"
#include "exiv2/preview.hpp"

#include "constants.h"

using namespace Exiv2;

Exiv2Lib::Exiv2Lib()
{

}



const QImage Exiv2Lib::thumbnail(const QString& path)
{
    Image::AutoPtr img;
    try{
        img = ImageFactory::open(path.toStdString());
    }
    catch(BasicError<char> e)
    {
        qDebug() << e.what();
        return QImage();
    }
    img->readMetadata();

    PreviewManager loader(*img);
    PreviewPropertiesList list = loader.getPreviewProperties();
    if (list.empty())
    {
        qDebug() << "Image at" << path << " contains no thumbnail";
        return QImage();
    }

    PreviewProperties selected;
    // Take the last image in the list.
    // For Canon raw pictures, the 2nd preview has no white balance applied.
    // see http://lclevy.free.fr/cr2/
    //     section 2.6 IFD #2
    selected = list.back();

    // load the thumbnail
    PreviewImage thumbnail = loader.getPreviewImage(selected);

    const unsigned char * tmp = thumbnail.pData();
    size_t size = thumbnail.size();

    QImage thumb;

    thumb.loadFromData(tmp,size);

    // Read the EXIF orientation flag and rotate the image accordingly.
    ExifData & data = img->exifData();
    ExifData::const_iterator pos;
    pos = data.findKey(ExifKey("Exif.Image.Orientation"));
    // TODO: should flip/rotate after resize
    if (pos != data.end() && pos->size() >0)
    {
        qDebug() << "rotation" << pos->toLong();
        QTransform rotate;
        switch(pos->toLong())
        {
        case 1: // no rotation needed
            break;
        case 2: // flip over y
            thumb = thumb.mirrored(true,false);
            break;
        case 3: // rotate 180 (or flip x flip y)
            thumb = thumb.mirrored(true,true);
            break;
        case 4: // flip over x
            thumb = thumb.mirrored(false,true);
            break;
        case 5: // rotate 90 CW & flip over y
            rotate.rotate(90);
            rotate.rotate(180,Qt::YAxis);
            thumb = thumb.transformed(rotate);
            break;
        case 6: // rotate 90 CW
            rotate.rotate(90);
            thumb = thumb.transformed(rotate);
            break;
        case 7: // rotate 90 CCW flip over y
            rotate.rotate(-90);
            rotate.rotate(180,Qt::YAxis);
            thumb = thumb.transformed(rotate);
            break;
        case 8: // rotate 90 CCW
            rotate.rotate(-90);
            thumb = thumb.transformed(rotate);
            break;

        }
    }
    return thumb;

}
