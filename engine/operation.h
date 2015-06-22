#ifndef OPERATION_H
#define OPERATION_H

#include <QObject>

class Operation
{
public:
    Operation();
    virtual ~Operation() {};

    // this operation operates on the whole array
    // change the contents of the array
    virtual void opArray(float *, int width, int height) {};

};

#endif // OPERATION_H
