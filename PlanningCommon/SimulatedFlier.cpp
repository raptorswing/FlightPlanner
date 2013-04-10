#include "SimulatedFlier.h"

//static
SimulatedFlierResults SimulatedFlier::simulate(const Wayset &wayset,
                                               const QSharedPointer<PlanningProblem> &problem)
{
    const qreal subSampleDist = 5.0;
    UAVParameters fudgeParams = problem->uavParameters();
    fudgeParams.setMinTurningRadius(fudgeParams.minTurningRadius() * 0.99);
    const Wayset subSample = wayset.resample(subSampleDist, fudgeParams);

    qreal maxScore = 0.0;
    qreal score = 0.0;

    //Sum-up performance on tasks
    foreach(const QSharedPointer<FlightTaskArea> area, problem->areas())
    {
        foreach(const QSharedPointer<FlightTask> task, area->tasks())
        {
            if (task->taskType() == "No-Fly Zone")
                continue;
            const qreal subScore = task->calculateFlightPerformance(subSample,
                                                                    area->geoPoly(),
                                                                    fudgeParams,
                                                                    false);
            score += subScore;
            maxScore += task->maxTaskPerformance();
        }
    }

    /*
     *Find all the points in the Wayset where progress is made on the tasks.
     *Summarize it as a start and end time.
     *Also, we check our timing constraints in these loops
    */
    Wayset subSubSample;
    QHash<QSharedPointer<FlightTask>, qreal> lastProgress;
    QHash<QSharedPointer<FlightTask>, qreal> progressStart;
    QHash<QSharedPointer<FlightTask>, qreal> progressEnd;
    QSet<QSharedPointer<FlightTask> > timingViolators;
    for (int i = 0; i < subSample.size(); i++)
    {
        subSubSample.append(subSample.at(i));
        const qreal dist = subSampleDist * i;
        const qreal time = dist / problem->uavParameters().airspeed();

        foreach(const QSharedPointer<FlightTaskArea> area, problem->areas())
        {
            foreach(const QSharedPointer<FlightTask> task, area->tasks())
            {
                if (task->taskType() == "No-Fly Zone")
                    continue;

                const qreal subScore = task->calculateFlightPerformance(subSubSample,
                                                                        area->geoPoly(),
                                                                        fudgeParams,
                                                                        false);

                /*
                 *If our subscore is zero at this point or if it didn't change then we
                 *didn't make any progress at this step
                */
                if (subScore == 0.0)
                    continue;
                else if (lastProgress.contains(task) && lastProgress.value(task) >= subScore)
                    continue;
                lastProgress.insert(task, subScore);

                if (!progressStart.contains(task))
                    progressStart.insert(task, time);

                progressEnd.insert(task, time);

                //If we made progress at this time then we need to check timing constraints
                bool timingSatisfied = false;
                foreach(const TimingConstraint& tc, task->timingConstraints())
                {
                    if (time < tc.start() || time > tc.end())
                        continue;
                    timingSatisfied = true;
                    break;
                }
                if (!timingSatisfied)
                    timingViolators.insert(task);
            }
        }
    }


    //Evaluate dependency constraints
    QSet<QSharedPointer<FlightTask> > dependencyViolators;
    foreach(const QSharedPointer<FlightTaskArea> area, problem->areas())
    {
        foreach(const QSharedPointer<FlightTask> task, area->tasks())
        {
            //If this task didn't make any progress then it can't have violated dependencies
            if (!progressStart.contains(task))
                continue;

            const qreal ourStartTime = progressStart.value(task);

            foreach(const QWeakPointer<FlightTask>& weakDep, task->dependencyConstraints())
            {
                QSharedPointer<FlightTask> dep = weakDep.toStrongRef();
                if (dep.isNull())
                    continue;

                //If our dependency didn't make any progress then we've trivially violated it
                //Also violated if we made progress before the dependency ended.
                if (!progressEnd.contains(dep)
                        || progressEnd.value(dep) > ourStartTime)
                {
                    dependencyViolators.insert(task);
                    break;
                }
            }
        }
    }

    qDebug() << "max possible score" << maxScore;

    SimulatedFlierResults toRet;
    toRet.points = score;
    toRet.pointsPossible = maxScore;
    toRet.dependencyViolations = dependencyViolators;
    toRet.timingViolations = timingViolators;

    return toRet;
}
