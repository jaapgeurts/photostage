#include "exivfacade.h"

#include "exiv2lib.h"

namespace PhotoStage
{
ExivFacade* ExivFacade::createExivReader()
{
    return new Exiv2Lib();
}

ExivFacade::ExivFacade()
{
}
}