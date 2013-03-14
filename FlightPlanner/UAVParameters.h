#ifndef UAVPARAMETERS_H
#define UAVPARAMETERS_H

#include <QtGlobal>
#include <QDataStream>

class UAVParameters
{
public:
    UAVParameters(qreal airspeed = 14.0, qreal minTurningRadius = 38.0, qreal waypointInterval = 30.0);

    qreal airspeed() const;
    qreal minTurningRadius() const;
    qreal waypointInterval() const;

    void setAirspeed(qreal nSpeed);
    void setMinTurningRadius(qreal nRad);
    void setWaypointInterval(qreal interval);

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
};

QDataStream& operator<<(QDataStream& stream, const UAVParameters& params);
QDataStream& operator>>(QDataStream& stream, UAVParameters& params);

#endif // UAVPARAMETERS_H
