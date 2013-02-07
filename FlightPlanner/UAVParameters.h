#ifndef UAVPARAMETERS_H
#define UAVPARAMETERS_H

#include <QtGlobal>

class UAVParameters
{
public:
    UAVParameters(qreal airspeed = 14.0, qreal minTurningRadius = 15.0, qreal waypointInterval = 30.0);

    qreal airspeed() const;
    qreal minTurningRadius() const;
    qreal waypointInterval() const;

    void setAirspeed(qreal nSpeed);
    void setMinTurningRadius(qreal nRad);
    void setWaypointInterval(qreal interval);

private:
    qreal _airspeed;
    qreal _minTurningRadius;
    qreal _waypointInterval;
};

#endif // UAVPARAMETERS_H
