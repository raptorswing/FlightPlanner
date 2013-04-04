#ifndef SIMULATEDFLIER_H
#define SIMULATEDFLIER_H

#include <QSharedPointer>

#include "PlanningCommon_global.h"
#include "PlanningProblem.h"
#include "Wayset.h"

class PLANNINGCOMMONSHARED_EXPORT SimulatedFlier
{
public:
    static bool simulate(const Wayset& wayset,
                         const QSharedPointer<PlanningProblem>& problem,
                         qreal * scoreOut = 0);
};

#endif // SIMULATEDFLIER_H
