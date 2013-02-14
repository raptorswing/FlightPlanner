#ifndef FLIGHTTASK_H
#define FLIGHTTASK_H

#include <QtGlobal>
#include <QObject>
#include <QPolygonF>
#include <QList>
#include <QString>

#include "Position.h"
#include "TimingConstraint.h"
#include "UAVParameters.h"

class FlightTask : public QObject
{
    Q_OBJECT
public:
    FlightTask();
    virtual ~FlightTask();

    virtual bool shortnessRewardApplies() const;

    virtual QString taskType() const=0;

    virtual qreal calculateFlightPerformance(const QList<Position>& positions,
                                             const QPolygonF& geoPoly,
                                             const UAVParameters& uavParams)=0;

    virtual qreal priority() const;

    virtual qreal maxTaskPerformance() const;

    const QList<TimingConstraint>& timingConstraints() const;
    void setTimingConstraints(const QList<TimingConstraint>& nConstraints);

    void addTimingConstraint(const TimingConstraint& nConstraint);
    void removeTimingConstraint(const TimingConstraint& nConstraint);
    void removeTimingConstraintAt(int index);

    const QString& taskName() const;
    void setTaskName(const QString& nName);

signals:
    void flightTaskChanged();

protected:
    static qreal normal(qreal x, qreal stdDev, qreal scaleFactor=1000.0);

private:
    QList<TimingConstraint> _timingConstraints;
    QString _taskName;
};

#endif // FLIGHTTASK_H
