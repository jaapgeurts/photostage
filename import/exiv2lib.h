#ifndef EXIV2_H
#define EXIV2_H

#include "exivfacade.h"

#include "exiv2/image.hpp"
#include "exiv2/preview.hpp"

namespace PhotoStage
{
    class Exiv2Lib : public ExivFacade
    {
        public:

            Exiv2Lib();

            void openFile(const QString& path);
            ExifInfo data();

        private:

            ExifInfo              mExifData;

            Exiv2::Image::AutoPtr mImageFile;

            const QImage loadImage();
            void setWhiteBalanceCoeffs(Exiv2::ExifData& data, float wb[]);
    };
}

#endif // EXIV2_H