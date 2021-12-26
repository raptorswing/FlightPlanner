#ifndef SIMULATEDFLIER_H
#define SIMULATEDFLIER_H

#include <QSharedPointer>

#include "PlanningCommon_global.h"
#include "PlanningProblem.h"
#include "Wayset.h"

typedef struct SimulatedFlierResults
{
    qreal points;
    qreal pointsPossible;
    QSet<QSharedPointer<FlightTask> > timingViolations;
    QSet<QSharedPointer<FlightTask> > dependencyViolations;
    QSet<QSharedPointer<FlightTaskArea> > noFlyViolations;
} SimulatedFlierResults;

class PLANNINGCOMMONSHARED_EXPORT SimulatedFlier
{
public:
    static SimulatedFlierResults simulate(const Wayset& wayset,
                                          const QSharedPointer<PlanningProblem>& problem);
};

#endif // SIMULATEDFLIER_H
