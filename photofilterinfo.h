#ifndef PHOTOSTAGE_PHOTOFILTERINFO_H
#define PHOTOSTAGE_PHOTOFILTERINFO_H

#include <QStringList>

#include "photo.h"

namespace PhotoStage
{
struct PhotoFilterInfo
{
    Photo::Flag       flag;
    Photo::ColorLabel colorLabel;
    uint8_t           rating;
    QStringList       keywords;
};
}
#endif // PHOTOSTAGE_PHOTOFILTERINFO_H
