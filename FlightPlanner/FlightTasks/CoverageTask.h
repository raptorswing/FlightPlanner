#ifndef COVERAGETASK_H
#define COVERAGETASK_H

#include <QObject>
#include <QVector>
#include <QVector3D>

#include "FlightTask.h"

class CoverageTask : public FlightTask
{
    Q_OBJECT
public:
    CoverageTask();

    virtual QString taskType() const;

    virtual qreal calculateFlightPerformance(const QList<Position>& positions,
                                             const QPolygonF& geoPoly);

    virtual qreal maxTaskPerformance() const;
    
//private:
    void _calculateBins(const QPolygonF& geoPoly);

    QPolygonF _lastGeoPoly;
    QVector<Position> _bins;
    QVector<QVector3D> _xyzBins;

    bool _debug;
    
};

#endif // COVERAGETASK_H
