#ifndef UAVPARAMETERS_H
#define UAVPARAMETERS_H

#include <QtGlobal>
#include <QDataStream>

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT UAVParameters
{
public:
    UAVParameters(qreal airspeed = 13.0,
                  qreal minTurningRadius = 30.0,
                  qreal waypointInterval = 30.0,
                  qreal directionalSensorViewAngle = 359.9,
                  bool radians = true);

    qreal airspeed() const;
    qreal minTurningRadius() const;
    qreal waypointInterval() const;
    qreal directionalSensorViewAngleRadians() const;
    qreal directionalSensorViewAngleDegrees() const;

    void setAirspeed(qreal nSpeed);
    void setMinTurningRadius(qreal nRad);
    void setWaypointInterval(qreal interval);
    void setDirectionalSensorViewAngleRadians(qreal angle);
    void setDirectionalSensorViewAngleDegrees(qreal angle);

    /**
     * @brief maxTurnAngle returns the maximum turn angle (in radians) that can be achieved in the distance
     * of the waypoint interval. Calculated using the minimum turning radius.
     * @return
     */
    qreal maxTurnAngle() const;

private:
    qreal _airspeed;
    qreal _minTurningRadius;
    qreal _waypointInterval;
    qreal _directionalSensorViewAngle;
};

QDataStream& operator<<(QDataStream& stream, const UAVParameters& params);
QDataStream& operator>>(QDataStream& stream, UAVParameters& params);

#endif // UAVPARAMETERS_H
