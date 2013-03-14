#include "UAVParameters.h"

#include <cmath>


UAVParameters::UAVParameters(qreal airspeed, qreal minTurningRadius, qreal waypointInterval) :
    _airspeed(airspeed), _minTurningRadius(minTurningRadius), _waypointInterval(waypointInterval)
{
}

qreal UAVParameters::airspeed() const
{
    return _airspeed;
}

qreal UAVParameters::minTurningRadius() const
{
    return _minTurningRadius;
}

qreal UAVParameters::waypointInterval() const
{
    return _waypointInterval;
}

void UAVParameters::setAirspeed(qreal nSpeed)
{
    _airspeed = nSpeed;
}

void UAVParameters::setMinTurningRadius(qreal nRad)
{
    _minTurningRadius = nRad;
}

void UAVParameters::setWaypointInterval(qreal interval)
{
    _waypointInterval = interval;
}

qreal UAVParameters::maxTurnAngle() const
{
    return _waypointInterval / _minTurningRadius;
}

//non-member
QDataStream& operator<<(QDataStream& stream, const UAVParameters& params)
{
    stream << params.airspeed();
    stream << params.minTurningRadius();
    stream << params.waypointInterval();

    return stream;
}

//non-member
QDataStream& operator>>(QDataStream& stream, UAVParameters& params)
{
    qreal airspeed, minTurningRadius, waypointInterval;

    stream >> airspeed;
    stream >> minTurningRadius;
    stream >> waypointInterval;

    UAVParameters a(airspeed, minTurningRadius, waypointInterval);
    params = a;

    return stream;
}
