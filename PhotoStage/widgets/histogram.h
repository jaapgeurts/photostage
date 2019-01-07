#ifndef WIDGETS_HISTOGRAM_H
#define WIDGETS_HISTOGRAM_H

#include <QWidget>

#define BIN_SIZE (256)

namespace Widgets
{
class Histogram : public QWidget
{
    Q_OBJECT

    public:

        explicit Histogram(QWidget* parent = 0);

        void setImageData(uint16_t* data, int width, int height);
        void clear();

    protected:

        void paintEvent(QPaintEvent*);

    signals:

    public slots:

        void recalculate();

    private:

        uint16_t* mData;
        int       mWidth;
        int       mHeight;

        uint32_t  mChannelRed[BIN_SIZE];
        uint32_t  mChannelGreen[BIN_SIZE];
        uint32_t  mChannelBlue[BIN_SIZE];

        uint32_t  mMaxAll;

        void reset();
};
}

#endif // WIDGETS_HISTOGRAM_H
