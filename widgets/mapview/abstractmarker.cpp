#include "abstractmarker.h"

namespace MapView
{
AbstractMarker::AbstractMarker(QObject* parent) : QObject(parent)
{
}

AbstractMarker::AbstractMarker(const QGeoCoordinate& coord, QObject* parent) :
    QObject(parent),
    mAnchor(CenterBottom),
    mCoord(coord)
{
}

AbstractMarker::~AbstractMarker()
{
}

void AbstractMarker::setCoord(const QGeoCoordinate& coord)
{
    mCoord = coord;
}

const QGeoCoordinate& AbstractMarker::coord() const
{
    return mCoord;
}

AbstractMarker::Anchor AbstractMarker::anchor() const
{
    return mAnchor;
}

void AbstractMarker::setAnchor(AbstractMarker::Anchor anchor)
{
    mAnchor = anchor;
}
}
