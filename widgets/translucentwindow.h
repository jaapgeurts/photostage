#ifndef TRANSLUCENTWINDOW_H
#define TRANSLUCENTWINDOW_H

#include <QWidget>
#include <QRect>

namespace PhotoStage
{
    class TranslucentWindow : public QWidget
    {
        public:

            TranslucentWindow(QWidget* parent = 0);

            void setGapGeometry(const QRect& rect);

        protected:

            virtual void paintEvent(QPaintEvent* paintEvent);

        private:

            QRect mGapRect;
    };
}
#endif // TRANSLUCENTWINDOW_H