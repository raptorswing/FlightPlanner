#ifndef SAMPLINGTASK_H
#define SAMPLINGTASK_H

#include "FlightTask.h"
#include "Wayset.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT SamplingTask : public FlightTask
{
    Q_OBJECT
public:
    explicit SamplingTask(const qreal timeRequired);

    //for de-serializing
    SamplingTask(QDataStream& stream);

    //pure-virtual from Serializable
    virtual QString serializationType() const;

    //pure-virtual from Serializable
    virtual void serialize(QDataStream& stream) const;

    virtual bool shortnessRewardApplies() const;

    virtual QString taskType() const;

    virtual qreal calculateFlightPerformance(const Wayset& wayset,
                                             const QPolygonF& geoPoly,
                                             const UAVParameters& uavParams,
                                             bool includeEnticement = true,
                                             qreal * progressStartOut = 0,
                                             qreal * progressEndOut = 0);

    virtual qreal maxTaskPerformance() const;

    qreal timeRequired() const;
    void setTimeRequired(qreal nTime);

protected:
    //virtual from FlightTask
    void _calculateBins(const QPolygonF& geoPoly);
    
private:
    qreal _timeRequired;
    
};

#endif // SAMPLINGTASK_H
