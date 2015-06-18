#include "pipeline.h"

Pipeline::Pipeline()
{
}

Pipeline::~Pipeline()
{

}

void Pipeline::setIntent(Pipeline::OutputIntent intent)
{
    mIntent = intent;
}

void Pipeline::setSource(Image* source)
{
    mSource = source;
}

void Pipeline::setDestination(Image* destination)
{
    mDestination = destination;
}

void Pipeline::execute()
{

}
