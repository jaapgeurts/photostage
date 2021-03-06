#ifndef WIDGETS_TIMEZONEPICKER_H
#define WIDGETS_TIMEZONEPICKER_H

#include <QGeoCoordinate>
#include <QHash>
#include <QList>
#include <QMenu>
#include <QPaintEvent>
#include <QPixmap>
#include <QResizeEvent>
#include <QStringList>
#include <QWidget>

namespace Widgets {
struct TimezoneArea {
  QString                      timezonename;
  QList<QList<QGeoCoordinate>> polygons;
  bool                         isHighlighted;
};

class TimezonePicker : public QWidget
{
  Q_OBJECT

public:
  TimezonePicker(QWidget* parent = 0);
  ~TimezonePicker();

signals:

  void homeTimezoneSelected(const QString& tzName);
  void destinationTimezoneSelected(const QString& tzName);

protected:
  void paintEvent(QPaintEvent*);
  void resizeEvent(QResizeEvent* event);

  void mouseMoveEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);

private slots:

  void plantHomeFlag();
  void plantDestinationFlag();

private:
  QPixmap                  mBackground;
  QPixmap                  mBackgroundScaled;
  QList<TimezoneArea*>     mTimezoneAreas;
  QHash<int, QStringList*> mCountryMap;
  QMenu*                   mMenu;
  QGeoCoordinate           mHomeFlagLocation;
  QGeoCoordinate           mDestinationFlagLocation;
  QPoint                   mLastClickLocation;
  QFont                    mFontGeneralFoundIcons;

  void          parseCountries();
  void          makePath(QPainterPath& path, const TimezoneArea* a) const;
  TimezoneArea* contains(const QPoint& pos) const;

  QHash<int, QStringList*> createMap() const;

  QGeoCoordinate pointToGeo(const QPoint& point) const;
  QPoint         geoToPoint(const QGeoCoordinate& coord) const;
};
} // namespace Widgets
#endif // TIMEZONEPICKER_H
