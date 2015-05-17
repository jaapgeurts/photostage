#ifndef ABSTRACTCELLRENDERER_H
#define ABSTRACTCELLRENDERER_H

#include <QPainter>
#include <QVariant>
#include <QMouseEvent>

class TileView;

struct TileInfo
{
    enum TileState {
        TileStateNone      = 0x000,
        TileStateSelected  = 0x001,
        TileStateActive    = 0x002,
        TileStateChecked   = 0x004,
        TileStateUnchecked = 0x008
    };

    TileInfo() { userdata = NULL; tileState = TileStateNone; }

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

    TileState tileState;

    void * userdata;

};

class AbstractTile
{
public:

    AbstractTile(TileView* parent=0);
    virtual ~AbstractTile() {};

    void setSize(const QSize& size) { mSize = size; }
    QSize size() { return mSize; }

    virtual void render(QPainter& painter,  TileInfo& info, const QVariant& data) = 0;
    virtual bool mouseMoveEvent(QMouseEvent*,  TileInfo&) { return false; }
    virtual bool mousePressEvent(QMouseEvent*,  TileInfo&) { return false; }
    virtual bool mouseReleaseEvent(QMouseEvent*,  TileInfo&) { return false; }


protected:
    const TileView* parent() const { return mParent; }
    void update();

private:
    TileView* mParent;
    QSize mSize;

};

#endif // ABSTRACTCELLRENDERER_H
