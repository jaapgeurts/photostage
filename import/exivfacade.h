#ifndef EXIVFACADE_H
#define EXIVFACADE_H

#include <QImage>

namespace PhotoStage
{
    struct ExifInfo
    {
        QString make;
        QString model;
        QImage thumbnail;
        float rgbCoeffients[3];
    };

    class ExivFacade
    {
        public:

            static ExivFacade* createExivReader();

            virtual ~ExivFacade()
            {
            };

            virtual void openFile(const QString& path) = 0;
            virtual ExifInfo data()                    = 0;

        protected:

            ExivFacade();
    };
}

#endif // EXIVFACADE_H