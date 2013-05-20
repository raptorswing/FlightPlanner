#include "AngleRange.h"

#include <QtGlobal>

const qreal PI = 3.141592653589793238462643383279;

AngleRange::AngleRange() : _rangeRadians(0)
{
}

AngleRange::AngleRange(const UAVOrientation &center, qreal range, bool radians) :
    _center(center)
{
    if (radians)
        this->setRangeRadians(range);
    else
        this->setRangeDegrees(range);
}

//for de-serializing
AngleRange::AngleRange(QDataStream &stream)
{
    _center = UAVOrientation(stream);
    stream >> _rangeRadians;
}

//pure virtual from Serializable
void AngleRange::serialize(QDataStream &stream) const
{
    _center.serialize(stream);
    stream << _rangeRadians;
}

//pure virtual from Serializable
QString AngleRange::serializationType() const
{
    return "AngleRange";
}

bool AngleRange::isNull() const
{
    //True if value is zero or "null"
    return (qFuzzyIsNull(_rangeRadians));
}

const UAVOrientation &AngleRange::center() const
{
    return _center;
}

void AngleRange::setCenter(const UAVOrientation &newCenter)
{
    _center = newCenter;
}

qreal AngleRange::rangeDegrees() const
{
    return _rangeRadians * 180.0 / PI;
}

qreal AngleRange::rangeRadians() const
{
    return _rangeRadians;
}

void AngleRange::setRangeDegrees(qreal nRangeDegrees)
{
    this->setRangeRadians(nRangeDegrees * PI / 180.0);
}

void AngleRange::setRangeRadians(qreal nRangeRadians)
{
    _rangeRadians = qBound<qreal>(0, nRangeRadians, 2.0 * PI);
}

bool AngleRange::withinRange(const UAVOrientation &orientation) const
{
    //Use dot product to get angle between
    const qreal dp = cos(orientation.radians()) * cos(_center.radians())
            + sin(orientation.radians()) * sin(_center.radians());

    //C++ acos should return from interval [0, pi]
    const qreal angleBetween = acos(dp);

    return angleBetween <= _rangeRadians / 2.0;
}
