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

    return thumb;

}
