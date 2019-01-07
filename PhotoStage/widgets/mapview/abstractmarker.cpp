#include "abstractmarker.h"

namespace MapView
{
AbstractMarker::AbstractMarker(QObject* parent) :
    QObject(parent),
    mAnchor(CenterBottom)
{
}

AbstractMarker::~AbstractMarker()
{
}

AbstractMarker::Anchor AbstractMarker::anchor() const
{
    return mAnchor;
}

void AbstractMarker::setAnchor(AbstractMarker::Anchor anchor)
{
    mAnchor = anchor;
}

MarkerInfo::MarkerInfo() : markerState(MarkerStateNone)
{
}
}
