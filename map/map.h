#ifndef MAP_H
#define MAP_H

#include "module.h"

namespace Ui
{
    class Map;
}

namespace PhotoStage
{
    class Map : public Module
    {
        Q_OBJECT

        public:

            explicit Map(QWidget* parent = 0);
            ~Map();

            QRect lightGap();

        private:

            Ui::Map* ui;
    };
}
#endif // MAP_H