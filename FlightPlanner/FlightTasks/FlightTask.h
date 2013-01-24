#ifndef FLIGHTTASK_H
#define FLIGHTTASK_H

#include <QtGlobal>
#include <QObject>
#include <QPolygonF>

#include "Position.h"

class FlightTask : public QObject
{
    Q_OBJECT
public:
    FlightTask();
    virtual ~FlightTask();

    virtual bool shortnessRewardApplies() const;

    virtual QString taskType() const=0;

    virtual qreal calculateFlightPerformance(const QList<Position>& positions,
                                             const QPolygonF& geoPoly)=0;

    virtual qreal priority() const;

    virtual qreal maxTaskPerformance() const;

signals:
    void flightTaskChanged();

protected:
    static qreal normal(qreal x, qreal stdDev, qreal scaleFactor=1000.0);
};

#endif // FLIGHTTASK_H
