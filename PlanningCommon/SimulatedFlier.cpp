#include "SimulatedFlier.h"

//static
bool SimulatedFlier::simulate(const Wayset &wayset,
                              const QSharedPointer<PlanningProblem> &problem,
                              qreal *scoreOut)
{
    UAVParameters fudgeParams = problem->uavParameters();
    fudgeParams.setMinTurningRadius(fudgeParams.minTurningRadius() * 0.99);
    const Wayset subSample = wayset.resample(5.0, fudgeParams);

    qreal maxScore = 0.0;
    qreal score = 0.0;

    //Sum-up performance on tasks
    foreach(const QSharedPointer<FlightTaskArea> area, problem->areas())
    {
        foreach(const QSharedPointer<FlightTask> task, area->tasks())
        {
            const qreal subScore = task->calculateFlightPerformance(subSample,
                                                                    area->geoPoly(),
                                                                    fudgeParams);
            score += subScore;
            maxScore += task->maxTaskPerformance();
        }
    }

    //Evaluate timing constraints


    //Evaluate dependency constraints



    qDebug() << "max possible score" << maxScore;

    if (scoreOut)
        *scoreOut = score;
    return true;
}
