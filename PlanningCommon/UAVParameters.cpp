#include "UAVParameters.h"

#include <cmath>


UAVParameters::UAVParameters(qreal airspeed,
                             qreal minTurningRadius,
                             qreal waypointInterval,
                             qreal directionalSensorViewAngle) :
    _airspeed(airspeed),
    _minTurningRadius(minTurningRadius),
    _waypointInterval(waypointInterval),
    _directionalSensorViewAngle(directionalSensorViewAngle)
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

qreal UAVParameters::directionalSensorViewAngle() const
{
    return _directionalSensorViewAngle;
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

void UAVParameters::setDirectionalSensorViewAngle(qreal angle)
{
    _directionalSensorViewAngle = angle;
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
    stream << params.directionalSensorViewAngle();

    return stream;
}

//non-member
QDataStream& operator>>(QDataStream& stream, UAVParameters& params)
{
    qreal airspeed, minTurningRadius, waypointInterval, directionalSensorViewAngle;

    stream >> airspeed;
    stream >> minTurningRadius;
    stream >> waypointInterval;
    stream >> directionalSensorViewAngle;

    UAVParameters a(airspeed, minTurningRadius, waypointInterval, directionalSensorViewAngle);
    params = a;

    return stream;
}
