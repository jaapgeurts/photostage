#ifndef IMAGEFILECELLRENDERER_H
#define IMAGEFILECELLRENDERER_H

#include "widgets/abstracttile.h"

namespace PhotoStage {
class ImageFileTile : public Widgets::AbstractTile
{
  Q_OBJECT

public:
  ImageFileTile(Widgets::TileView* parent = 0);

  void render(QPainter& painter, const Widgets::TileInfo& tileInfo,
              const QVariant& data);

private:
};
} // namespace PhotoStage

#endif // IMAGEFILECELLRENDERER_H
