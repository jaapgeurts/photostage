#include "exivfacade.h"

#include "exiv2lib.h"

ExivFacade *ExivFacade::createExivReader()
{
  return new Exiv2Lib();
}

ExivFacade::ExivFacade()
{

}

