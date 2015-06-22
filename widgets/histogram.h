#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QWidget>
#include <QImage>

#define BIN_SIZE (256)

namespace PhotoStage
{
    class Histogram : public QWidget
    {
        Q_OBJECT

        public:

            explicit Histogram(QWidget* parent = 0);

            void setImageData(const QImage& image);

        protected:

            void paintEvent(QPaintEvent* event);

        signals:

        public slots:

            void recalculate();

        private:

            QImage        mImage;

            unsigned long mChannelRed[BIN_SIZE];
            unsigned long mChannelGreen[BIN_SIZE];
            unsigned long mChannelBlue[BIN_SIZE];

            unsigned long mMaxAll;
    };
}

#endif // HISTOGRAM_H