#include <QPainter>
#include <QTextStream>
#include <QTimeZone>

#include <QDebug>

#include "timezonepicker.h"

namespace Widgets
{
TimezonePicker::TimezonePicker(QWidget* parent) :
    QWidget(parent),
    mBackground(":/images/worldmap.jpg")
{
    parseCountries();

    setMouseTracking(true);

    QTimeZone tz("Asia/Ho_Chi_Minh");
    qDebug() << "HCM offset:" << tz.standardTimeOffset(QDateTime::currentDateTime());
    qDebug() << "+7:" << QTimeZone::availableTimeZoneIds(25200);

    mCountryMap = createMap();
}

TimezonePicker::~TimezonePicker()
{
    qDeleteAll(mTimezoneAreas);
    mTimezoneAreas.clear();

    qDeleteAll(mCountryMap.values());
    mCountryMap.clear();
}

void TimezonePicker::parseCountries()
{
    QFile data(":/data/tz_world.txt");

    if (!data.open(QFile::ReadOnly | QFile::Text))
        throw std::runtime_error("Can't open worldmap timezone data");
    QTextStream in(&data);

    while (!in.atEnd())
    {
        TimezoneArea* tzArea = new TimezoneArea();
        QString       line   = in.readLine();
        QStringList   list   = line.split('|');
        tzArea->timezonename = list.at(0);
        line                 = list.at(1);
        line                 = line.mid(12);

        QList<QGeoCoordinate> polygon;
        enum
        {
            MULTIPOLYGON = 1,
            POLYGONGROUP,
            COORDINATELIST,
            LONGITUDE,
            LATITUDE,
            FAIL,
        }
        state          = MULTIPOLYGON;
        int     i      = 0;
        int     length = line.length();
        QString tmp;
        double  longitude = 0;

        while (i < length)
        {
            QChar l = line.at(i);

            switch (state)
            {
                case MULTIPOLYGON:

                    if (l == '(')
                        state = POLYGONGROUP;
                    else
                    {
                        qDebug() << tzArea->timezonename << "1. expected '(' or ')' found " << l;
                        state = FAIL;
                    }
                    break;

                case POLYGONGROUP:

                    if (l == '(')
                        state = COORDINATELIST;
                    else if (l == ')')
                    {
                        // qDebug() << "Adding country/timezone" << tzArea->timezonename;
                        mTimezoneAreas.append(tzArea);
                    }
                    else
                    {
                        qDebug() << tzArea->timezonename << "2. expected '(' or ')' found " << l;
                        state = FAIL;
                    }
                    break;

                case COORDINATELIST:

                    if (l == '(')
                    {
                        polygon = QList<QGeoCoordinate>();
                        state   = LONGITUDE;
                    }
                    else if (l == ',')
                    {
                        tzArea->polygons.append(polygon);
                    }
                    else if (l == ')')
                    {
                        tzArea->polygons.append(polygon);
                        state = POLYGONGROUP;
                    }
                    else
                    {
                        qDebug() << tzArea->timezonename << "4. expected '(' or ')' or ',' found " << l;
                        state = FAIL;
                    }
                    break;

                case LONGITUDE:

                    if (l == '-' || l == '.' || (l >= '0' && l <= '9'))
                    {
                        tmp += l;
                    }
                    else if (l == ' ')
                    {
                        // read longitude
                        longitude = tmp.toDouble();
                        tmp.clear();
                        state = LATITUDE;
                    }
                    else
                    {
                        qDebug() << tzArea->timezonename << "5. expected '[0-9.-]' or ' ' found " << l;
                        state = FAIL;
                    }
                    break;

                case LATITUDE:

                    if (l == '-' || l == '.' || (l >= '0' && l <= '9'))
                    {
                        tmp += l;
                    }
                    else if (l == ',')
                    {
                        // read latitude
                        polygon.append(QGeoCoordinate(tmp.toDouble(), longitude));
                        tmp.clear();
                        state = LONGITUDE;
                    }
                    else if (l == ')')
                    {
                        // read latitude
                        polygon.append(QGeoCoordinate(tmp.toDouble(), longitude));
                        tmp.clear();
                        state = COORDINATELIST;
                    }
                    else
                    {
                        qDebug() << tzArea->timezonename << "6. expected '[0-9.-]' or ',' or ')' found " << l;
                        state = FAIL;
                    }
                    break;

                case FAIL:
                    qDebug() << "Skipping line" << tzArea->timezonename;
                    break;
            }
            i++;
        }
    }

    data.close();
}

void TimezonePicker::mouseMoveEvent(QMouseEvent* event)
{
    TimezoneArea* tzArea = contains(event->pos());

    if (tzArea != NULL)
    {
        // now we know the country.
        // get the timezone for this country
        QTimeZone tz(tzArea->timezonename.toLocal8Bit());

        // now get all countries for this timezone
        QStringList* countries =  mCountryMap.value(tz.standardTimeOffset(QDateTime::currentDateTime()));

        foreach(TimezoneArea * a, mTimezoneAreas)
        {
            a->isHighlighted = false;

            foreach(QString ba, *countries)
            {
                if (a->timezonename == ba)
                    a->isHighlighted = true;
            }
        }
    }

    update();
}

QHash<int, QStringList*> TimezonePicker::createMap()
{
    QHash<int, QStringList*> map;

    QList<QByteArray>        ianaList = QTimeZone::availableTimeZoneIds();
    foreach(QByteArray a, ianaList)
    {
        QTimeZone z(a);

        int       offset = z.standardTimeOffset(QDateTime::currentDateTime());

        if (!map.contains(offset))
            map.insert(offset, new QStringList());

        map.value(offset)->append(QString(a));
    }

    return map;
}

TimezoneArea* TimezonePicker::contains(const QPoint& pos)
{
    foreach(TimezoneArea * a, mTimezoneAreas)
    {
        QPainterPath path;

        makePath(path, a);

        if (path.contains(pos))
            return a;
    }
    return NULL;
}

void TimezonePicker::makePath(QPainterPath& path, const TimezoneArea* a)
{
    //        if (!a.timezonename.startsWith("Europe"))
    //            continue;
    // only draw main polygon. (ignore the cutout polygons
    const QList<QGeoCoordinate> list = a->polygons.at(0);
    const QGeoCoordinate        c1   = list.at(0);
    int                         x1   = (c1.longitude() + 180.0) / 360.0 * mBackgroundScaled.width();
    int                         y1   = mBackgroundScaled.height() -
        (c1.latitude() + 90.0) / 180.0 * mBackgroundScaled.height();

    path.moveTo(x1, y1);

    int i    = 1;
    int size = list.size();

    while (i < size)
    {
        const QGeoCoordinate c2 = list.at(i);

        int                  x2 = (c2.longitude() + 180.0) / 360.0 * mBackgroundScaled.width();
        int                  y2 = mBackgroundScaled.height() -
            (c2.latitude() + 90.0) / 180.0 * mBackgroundScaled.height();

        if (x1 != x2 || y1 != y2) // don't draw what we can't see
            path.lineTo(x2, y2);

        i++;
    }
    path.closeSubpath();
}

void TimezonePicker::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    painter.drawPixmap(0, 0, mBackgroundScaled);
    painter.setPen(Qt::black);

    foreach(TimezoneArea * a, mTimezoneAreas)
    {
        QPainterPath path;

        makePath(path, a);

        if (a->isHighlighted)
            painter.fillPath(path, QBrush(Qt::blue));
        else
            painter.drawPath(path);
    }
}

void TimezonePicker::resizeEvent(QResizeEvent* event)
{
    mBackgroundScaled = mBackground.scaled(event->size(), Qt::KeepAspectRatio);
    update();
}
}
