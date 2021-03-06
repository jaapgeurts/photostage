#ifndef ABSTRACTCELLRENDERER_H
#define ABSTRACTCELLRENDERER_H

#include <QModelIndex>
#include <QMouseEvent>
#include <QObject>
#include <QPainter>
#include <QVariant>

namespace Widgets {
class TileView;

struct TileInfo {
  enum TileState {
    TileStateNone      = 0x000,
    TileStateSelected  = 0x001,
    TileStateActive    = 0x002,
    TileStateChecked   = 0x004,
    TileStateUnchecked = 0x008
  };

  TileInfo();

  // the ordinal number in the list
  int index;
  // the matrix position in the grid
  int row;
  int column;
  // the pixel position in the view (relative to the viewport)
  int x;
  int y;
  int width;
  int height;

  QModelIndex modelIndex;

  TileState tileState;
};

class AbstractTile : public QObject
{
  Q_OBJECT

public:
  enum ModelRole { ImageRole = Qt::UserRole + 300 };

  AbstractTile(TileView* parent = 0);
  virtual ~AbstractTile();

  void  setSize(const QSize& size);
  QSize size() const;

  virtual int modelRole() const;

  virtual void render(QPainter& painter, const TileInfo& info,
                      const QVariant& data) = 0;

  virtual void mouseMoveEvent(QMouseEvent* e, const TileInfo&);
  virtual void mousePressEvent(QMouseEvent* e, const TileInfo&);
  virtual void mouseReleaseEvent(QMouseEvent* e, const TileInfo&);
  virtual void mouseEnterEvent(const TileInfo&);
  virtual void mouseLeaveEvent(const TileInfo&);

protected:
  void update();

private:
  QSize mSize;
};
} // namespace Widgets

#endif // ABSTRACTCELLRENDERER_H
