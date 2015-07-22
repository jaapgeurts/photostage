#include "runnable.h"

namespace PhotoStage
{
Runnable::Runnable()
    : mAutoDelete(true)
{
}

Runnable::~Runnable()
{
}

void Runnable::cancel()
{
}

bool Runnable::autoDelete() const
{
    return mAutoDelete;
}

void Runnable::setAutoDelete(bool enable)
{
    mAutoDelete = enable;
}
}
