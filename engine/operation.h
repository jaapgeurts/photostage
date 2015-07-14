#ifndef PHOTOSTAGE_OPERATION_H
#define PHOTOSTAGE_OPERATION_H

#include <QObject>

namespace PhotoStage
{
    class Operation
    {
        public:

            Operation();
            virtual ~Operation()
            {
            };

            // this operation operates on the whole array
            // change the contents of the array
            virtual void opArray(float*, int width, int height)
            {
            };
    };
}
#endif // PHOTOSTAGE_OPERATION_H
