#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QWidget>
#include <QImage>

#define BIN_SIZE (256)

namespace Widgets
{
class Histogram : public QWidget
{
    Q_OBJECT

    public:

        explicit Histogram(QWidget* parent = 0);

        void setImageData(const QImage& image);
        void clear();

    protected:

        void paintEvent(QPaintEvent*);

    signals:

    public slots:

        void recalculate();

    private:

        QImage   mImage;

        uint32_t mChannelRed[BIN_SIZE];
        uint32_t mChannelGreen[BIN_SIZE];
        uint32_t mChannelBlue[BIN_SIZE];

        uint32_t mMaxAll;

        void reset();
};
}

#endif // HISTOGRAM_H
