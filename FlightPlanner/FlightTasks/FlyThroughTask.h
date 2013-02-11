#ifndef FLYTHROUGHTASK_H
#define FLYTHROUGHTASK_H

#include "FlightTask.h"

class FlyThroughTask : public FlightTask
{
    Q_OBJECT
public:
    FlyThroughTask();

    virtual QString taskType() const;

    virtual qreal calculateFlightPerformance(const QList<Position>& positions,
                                             const QPolygonF& geoPoly,
                                             const UAVParameters& uavParams);
    
signals:
    
public slots:
    
};

#endif // FLYTHROUGHTASK_H
