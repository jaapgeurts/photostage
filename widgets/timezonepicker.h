#ifndef WIDGETS_TIMEZONEPICKER_H
#define WIDGETS_TIMEZONEPICKER_H

#include <QStringList>
#include <QWidget>
#include <QPaintEvent>
#include <QPixmap>
#include <QResizeEvent>
#include <QList>
#include <QGeoCoordinate>
#include <QHash>

namespace Widgets
{
struct TimezoneArea
{
    QString                       timezonename;
    QList<QList<QGeoCoordinate> > polygons;
    bool                          isHighlighted;
};

class TimezonePicker : public QWidget
{
    public:

        TimezonePicker(QWidget* parent = 0);
        ~TimezonePicker();

    protected:

        void paintEvent(QPaintEvent* event);
        void resizeEvent(QResizeEvent* event);

        void mouseMoveEvent(QMouseEvent* event);

    private:

        QPixmap                  mBackground;
        QPixmap                  mBackgroundScaled;
        QList<TimezoneArea*>     mTimezoneAreas;
        QHash<int, QStringList*> mCountryMap;

        void parseCountries();
        void makePath(QPainterPath& path, const TimezoneArea* a);
        TimezoneArea* contains(const QPoint& pos);

        QHash<int, QStringList*> createMap();
};
}
#endif // TIMEZONEPICKER_H
