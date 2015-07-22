#include "runnable.h"

namespace PhotoStage
{
Runnable::Runnable() :
    mAutoDelete(true),
    mName("Unnamed runnable")
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

void Runnable::setName(const QString& name)
{
    mName = name;
}

const QString& Runnable::name() const
{
    return mName;
}
}
