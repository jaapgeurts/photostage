#ifndef MAPVIEW_GROUPMARKER_H
#define MAPVIEW_GROUPMARKER_H

#include "abstractmarker.h"

namespace MapView
{
class GroupMarker : public AbstractMarker
{
    public:

        GroupMarker();

        void addMarker(const AbstractMarker& marker);
        void setProxyMarker(const AbstractMarker& marker);
};
}
#endif // GROUPMARKER_H
