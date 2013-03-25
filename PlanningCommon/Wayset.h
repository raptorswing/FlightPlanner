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

    Position& first();
    const Position& first() const;

    Position& last();
    const Position& last() const;

    int count() const;
    int size() const;
    bool isEmpty() const;

    qreal lengthMeters(const UAVParameters& uavParams) const;
    qreal timeToFly(const UAVParameters& uavParams) const;

    void clear();
    void append(const Position& pos);
    void append(const Wayset& wayset);
    void prepend(const Position& pos);

    const QList<Position>& waypoints() const;

private:
    QList<Position> _waypoints;
};

#endif // WAYSET_H
