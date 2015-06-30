#ifndef DEVELOPMODULE_H
#define DEVELOPMODULE_H

#include <QWidget>
#include "photo.h"

namespace PhotoStage
{
    class DevelopModule : public QWidget
    {
        Q_OBJECT

        public:

            explicit DevelopModule(QWidget* parent = 0);

            virtual void setPhoto(Photo photo);
            const Photo photo() const;

        signals:

            // TODO: remove when the engine takes care of rendering
            void parametersAdjusted();

        protected:

            Photo mPhoto;
    };
}
#endif // DEVELOPMODULE_H
