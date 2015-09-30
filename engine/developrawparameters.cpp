#include "developrawparameters.h"

namespace PhotoStage
{
DevelopRawParameters::DevelopRawParameters()
{
}

DevelopRawParameters::DevelopRawParameters(const ExifInfo& ex_info) :
    d(QSharedPointer<DevelopRawParametersPrivate>(new DevelopRawParametersPrivate(-1, ex_info)))
{
}

DevelopRawParameters::DevelopRawParameters(long long id, float red, float green, float blue) :
    d(QSharedPointer<DevelopRawParametersPrivate>(new DevelopRawParametersPrivate(id, red, green, blue)))
{
}

long long DevelopRawParameters::id() const
{
    return d->mId;
}

float DevelopRawParameters::redMultiplier() const
{
    return d->mRedMultiplier;
}

float DevelopRawParameters::greenMultiplier() const
{
    return d->mGreenMultiplier;
}

float DevelopRawParameters::blueMultiplier() const
{
    return d->mBlueMultiplier;
}

void DevelopRawParameters::setRotation(DevelopRawParameters::Rotation rotation)
{
    d->mRotation = rotation;
}

DevelopRawParameters::Rotation DevelopRawParameters::rotation() const
{
    return d->mRotation;
}

void DevelopRawParameters::setInterpolationAlgorithm(DevelopRawParameters::InterpolationAlgorithm algorithm)
{
    d->mAlgorithm = algorithm;
}

DevelopRawParameters::InterpolationAlgorithm DevelopRawParameters::interpolationAlgorithm() const
{
    return d->mAlgorithm;
}

bool DevelopRawParameters::isNull() const
{
    return d.isNull();
}

DevelopRawParameters::DevelopRawParametersPrivate::DevelopRawParametersPrivate(long long id,
    float red, float green, float blue) :
    mId(id),
    mRedMultiplier(red),
    mGreenMultiplier(green),
    mBlueMultiplier(blue)
{
}

DevelopRawParameters::DevelopRawParametersPrivate::DevelopRawParametersPrivate(long long id, const ExifInfo& ex_info) :
    mId(id),
    mRedMultiplier(ex_info.rgbCoeffients[0]),
    mGreenMultiplier(ex_info.rgbCoeffients[1]),
    mBlueMultiplier(ex_info.rgbCoeffients[2]),
    mRotation((DevelopRawParameters::Rotation)ex_info.rotation)
{
}
}
