#include "UAVParameters.h"

#include <cmath>

const qreal PI = 3.141592653589793238462643383;

UAVParameters::UAVParameters(qreal airspeed,
                             qreal minTurningRadius,
                             qreal waypointInterval,
                             qreal directionalSensorViewAngle,
                             bool radians) :
    _airspeed(airspeed),
    _minTurningRadius(minTurningRadius),
    _waypointInterval(waypointInterval)
{
    if (radians)
        this->setDirectionalSensorViewAngleRadians(directionalSensorViewAngle);
    else
        this->setDirectionalSensorViewAngleDegrees(directionalSensorViewAngle);
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

qreal UAVParameters::directionalSensorViewAngleRadians() const
{
    return _directionalSensorViewAngle;
}

qreal UAVParameters::directionalSensorViewAngleDegrees() const
{
    return _directionalSensorViewAngle * 180.0 / PI;
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

void UAVParameters::setDirectionalSensorViewAngleRadians(qreal angle)
{
    _directionalSensorViewAngle = qBound<qreal>(0, angle, 2.0 * PI);
}

void UAVParameters::setDirectionalSensorViewAngleDegrees(qreal angle)
{
    this->setDirectionalSensorViewAngleRadians(angle * PI / 180.0);
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
    stream << params.directionalSensorViewAngleRadians();

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
