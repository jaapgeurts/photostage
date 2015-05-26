#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QWidget>

#define BIN_SIZE (256)

class Histogram : public QWidget
{
    Q_OBJECT
public:
    explicit Histogram(QWidget *parent = 0);

    void setImageData(const QImage & image);

protected:
    void paintEvent(QPaintEvent * event);

signals:

public slots:

private:
    unsigned long mChannelRed[BIN_SIZE];
    unsigned long mChannelGreen[BIN_SIZE];
    unsigned long mChannelBlue[BIN_SIZE];

    unsigned long mMaxRed;
    unsigned long mMaxGreen;
    unsigned long mMaxBlue;

    unsigned long mMaxAll;

};

#endif // HISTOGRAM_H
