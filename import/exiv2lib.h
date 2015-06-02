#ifndef EXIV2_H
#define EXIV2_H

#include "exivfacade.h"

#include "exiv2/image.hpp"
#include "exiv2/preview.hpp"

using namespace Exiv2;


class Exiv2Lib : public ExivFacade
{
public:
    Exiv2Lib();

    void openFile(const QString& path);
    ExifInfo data();

private:
    ExifInfo mExifData;

    Image::AutoPtr mImageFile;


    const QImage loadImage();
    void setWhiteBalanceCoeffs(ExifData &data, float wb[]);
};

#endif // EXIV2_H
