#ifndef EXIV2_H
#define EXIV2_H

#include "exivfacade.h"

class Exiv2Lib : public ExivFacade
{
public:
    Exiv2Lib();


public:
    const QImage thumbnail(const QString& path);
};

#endif // EXIV2_H
