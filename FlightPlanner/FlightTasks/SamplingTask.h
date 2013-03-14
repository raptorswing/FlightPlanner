#ifndef SAMPLINGTASK_H
#define SAMPLINGTASK_H

#include "FlightTask.h"

class SamplingTask : public FlightTask
{
    Q_OBJECT
public:
    explicit SamplingTask(const qreal timeRequired);

    //for de-serializing
    SamplingTask(QDataStream& stream);

    //pure-virtual from Serializable
    virtual QString serializationKey() const;

    //pure-virtual from Serializable
    virtual void serialize(QDataStream& stream) const;

    virtual bool shortnessRewardApplies() const;

    virtual QString taskType() const;

    virtual qreal calculateFlightPerformance(const QList<Position>& positions,
                                             const QPolygonF& geoPoly,
                                             const UAVParameters& uavParams);

    virtual qreal maxTaskPerformance() const;

    qreal timeRequired() const;
    void setTimeRequired(qreal nTime);
    
private:
    qreal _timeRequired;
    
};

#endif // SAMPLINGTASK_H
