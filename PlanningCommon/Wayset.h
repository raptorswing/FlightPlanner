#ifndef WAYSET_H
#define WAYSET_H

#include <QList>

#include "Position.h"
#include "UAVParameters.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT Wayset
{
public:
    Wayset();
    Wayset(const QList<Position>& waypoints);

    const Position& at(int i) const;

    int count() const;
    qreal lengthMeters(const UAVParameters& uavParams) const;
    qreal timeToFly(const UAVParameters& uavParams) const;

    const QList<Position>& waypoints() const;

private:
    QList<Position> _waypoints;
};

#endif // WAYSET_H
