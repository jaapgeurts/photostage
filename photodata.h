#ifndef PHOTODATA
#define PHOTODATA
#include <cstdint>
#include <QSize>
#include <QSharedPointer>
#define NUM_CHANNELS (3)
class PhotoData
{
    public:

        PhotoData();
        explicit PhotoData(const int w, const int h);
        explicit PhotoData(const QSize& size);
        ~PhotoData();
        uint16_t* data();
        const uint16_t* constData() const;
        uint16_t* scanLine(int i);
        int width() const;
        int height() const;
        int length() const;
        PhotoData& operator=(const PhotoData& other);

    private:

        QSharedPointer<uint16_t> mData;
        int                      mWidth;
        int                      mHeight;
};

#endif // PHOTODATA