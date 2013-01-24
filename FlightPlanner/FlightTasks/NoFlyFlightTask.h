#ifndef NOFLYFLIGHTTASK_H
#define NOFLYFLIGHTTASK_H

#include "FlightTask.h"

class NoFlyFlightTask : public FlightTask
{
    Q_OBJECT
public:
    NoFlyFlightTask();

    //virtual from FlightTask
    virtual bool shortnessRewardApplies() const;

    //pure-virtual from FlightTask
    virtual QString taskType() const;

    //pure-virtual from FlightTask
    virtual qreal calculateFlightPerformance(const QList<Position>& positions,
                                             const QPolygonF& geoPoly);
};

#endif // NOFLYFLIGHTTASK_H
