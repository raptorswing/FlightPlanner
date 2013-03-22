#ifndef FLYTHROUGHTASK_H
#define FLYTHROUGHTASK_H

#include "FlightTask.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT FlyThroughTask : public FlightTask
{
    Q_OBJECT
public:
    FlyThroughTask();

    //for de-serializing
    FlyThroughTask(QDataStream& stream);

    //pure-virtual from Serializable
    virtual QString serializationType() const;

    //pure-virtual from Serializable
    virtual void serialize(QDataStream& stream) const;

    virtual QString taskType() const;

    virtual qreal calculateFlightPerformance(const QList<Position>& positions,
                                             const QPolygonF& geoPoly,
                                             const UAVParameters& uavParams);
    
signals:
    
public slots:
    
};

#endif // FLYTHROUGHTASK_H
