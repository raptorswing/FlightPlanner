#ifndef COVERAGETASK_H
#define COVERAGETASK_H

#include <QObject>
#include <QList>
#include <QVector3D>

#include "FlightTask.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT CoverageTask : public FlightTask
{
    Q_OBJECT
public:
    CoverageTask(qreal coverageGranularity = 100.0);

    //for de-serializing
    CoverageTask(QDataStream& stream);

    //pure-virtual from Serializable
    virtual QString serializationType() const;

    //pure-virtual from Serializable
    virtual void serialize(QDataStream& stream) const;

    virtual QString taskType() const;

    virtual qreal calculateFlightPerformance(const Wayset& wayset,
                                             const QPolygonF& geoPoly,
                                             const UAVParameters& uavParams,
                                             bool includeEnticement = true,
                                             qreal * progressStartOut = 0,
                                             qreal * progressEndOut = 0);

    virtual qreal maxTaskPerformance() const;

    qreal granularity() const;
    void setGranularity(qreal nGran);

protected:
    //virtual from FlightTask
    void _calculateBins(const QPolygonF& geoPoly);
    
private:
    qreal _granularity;
    
};

#endif // COVERAGETASK_H
