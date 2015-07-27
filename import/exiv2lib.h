#ifndef EXIV2_H
#define EXIV2_H

#include "exivfacade.h"

#include "exiv2/image.hpp"
#include "exiv2/preview.hpp"

#include "nullable.h"

namespace PhotoStage
{
class Exiv2Lib : public ExivFacade
{
    public:

        Exiv2Lib();

        bool openFile(const QString& path);
        QImage getPreview();
        ExifInfo data();

    private:

        ExifInfo              mExifInfo;

        Exiv2::Image::AutoPtr mImageFile;

        void setWhiteBalanceCoeffsCanon(Exiv2::ExifData& data, float wb[]);
        void setWhiteBalanceCoeffsNikon(Exiv2::ExifData& data, float wb[]);
        void readCanonNotes(Exiv2::ExifData& data);
        void readNikonNotes(Exiv2::ExifData& data);

};
}

#endif // EXIV2_H
