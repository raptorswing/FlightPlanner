#ifndef NOFLYFLIGHTTASK_H
#define NOFLYFLIGHTTASK_H

#include "FlightTask.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT NoFlyFlightTask : public FlightTask
{
    Q_OBJECT
public:
    NoFlyFlightTask();

    //for de-serializing
    NoFlyFlightTask(QDataStream& stream);

    //pure-virtual from Serializable
    virtual QString serializationType() const;

    //pure-virtual from Serializable
    virtual void serialize(QDataStream& stream) const;

    //virtual from FlightTask
    virtual bool shortnessRewardApplies() const;

    //pure-virtual from FlightTask
    virtual QString taskType() const;

    //pure-virtual from FlightTask
    virtual qreal calculateFlightPerformance(const Wayset& positions,
                                             const QPolygonF& geoPoly,
                                             const UAVParameters& uavParams);
};

#endif // NOFLYFLIGHTTASK_H
