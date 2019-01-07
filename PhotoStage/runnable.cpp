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

void Runnable::setId(uint32_t id)
{
    mId = id;
}

uint32_t Runnable::id() const
{
    return mId;
}
}
