#ifndef IMAGEFILECELLRENDERER_H
#define IMAGEFILECELLRENDERER_H

#include "widgets/abstracttile.h"

namespace PhotoStage
{
    class ImageFileTile : public TileView::AbstractTile
    {
        Q_OBJECT

        public:

            ImageFileTile(TileView::TileView* parent = 0);

            void render(QPainter& painter,
                const TileView::TileInfo& tileInfo,
                const QVariant& data);

        private:
    };
}

#endif // IMAGEFILECELLRENDERER_H
