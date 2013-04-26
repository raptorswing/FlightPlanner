#include "SimulatedFlier.h"

//static
SimulatedFlierResults SimulatedFlier::simulate(const Wayset &wayset,
                                               const QSharedPointer<PlanningProblem> &problem)
{
    qreal maxScore = 0.0;
    qreal score = 0.0;

    UAVParameters fudgeParams = problem->uavParameters();
    fudgeParams.setMinTurningRadius(fudgeParams.minTurningRadius() * 0.98);

    QHash<QSharedPointer<FlightTask>, qreal> taskStartTimes;
    QHash<QSharedPointer<FlightTask>, qreal> taskEndTimes;

    QList<QSharedPointer<FlightTaskArea> > obstacles;

    //Sum-up performance on tasks
    foreach(const QSharedPointer<FlightTaskArea> area, problem->areas())
    {
        foreach(const QSharedPointer<FlightTask> task, area->tasks())
        {
            if (task->taskType() == "No-Fly Zone")
            {
                obstacles.append(area);
                continue;
            }

            qreal taskStart = -1;
            qreal taskEnd = -1;
            const qreal subScore = task->calculateFlightPerformance(wayset,
                                                                    area->geoPoly(),
                                                                    fudgeParams,
                                                                    false,
                                                                    &taskStart, &taskEnd);
            score += subScore;
            maxScore += task->maxTaskPerformance();

            qDebug() << "Task progress for" << task->taskName() << " occurred during" << taskStart << taskEnd;
            if (taskStart != -1 && taskEnd != -1)
            {
                taskStartTimes.insert(task, taskStart);
                taskEndTimes.insert(task, taskEnd);
            }
        }
    }


    //Figure out timing and dependency constraint violations
    QSet<QSharedPointer<FlightTask> > timingViolators;
    QSet<QSharedPointer<FlightTask> > dependencyViolators;
    foreach(const QSharedPointer<FlightTaskArea> area, problem->areas())
    {
        foreach(const QSharedPointer<FlightTask> task, area->tasks())
        {
            if (!taskStartTimes.contains(task))
                continue;

            qreal startTime = taskStartTimes.value(task);
            qreal endTime = taskEndTimes.value(task);

            const QList<TimingConstraint>& constraints = task->timingConstraints();

            bool timingSatisfied = false;
            foreach(const TimingConstraint& tc, constraints)
            {
                if (startTime < tc.start() || startTime > tc.end())
                    continue;
                else if (endTime < tc.start() || endTime > tc.end())
                    continue;
                else
                {
                    timingSatisfied = true;
                    break;
                }
            }

            if (!timingSatisfied)
                timingViolators.insert(task);


            const QList<QWeakPointer<FlightTask> >& dependencies = task->dependencyConstraints();
            foreach(const QWeakPointer<FlightTask>& dep, dependencies)
            {
                QSharedPointer<FlightTask> strong = dep.toStrongRef();
                if (strong.isNull())
                    continue;

                if (!taskStartTimes.contains(strong))
                    dependencyViolators.insert(task);
                else if (startTime <= taskEndTimes.value(strong))
                    dependencyViolators.insert(task);
            }

        }
    }

    //Find no-fly/obstacle violations
    QSet<QSharedPointer<FlightTaskArea> > noFlyViolations;
    Wayset resampled = wayset.resample(problem->uavParameters().waypointInterval(),
                                       problem->uavParameters());
    foreach(const Position& pos, resampled.positions())
    {
        const QPointF lonLat = pos.lonLat();
        foreach(const QSharedPointer<FlightTaskArea>& area, obstacles)
        {
            const QPolygonF& obsPoly = area->geoPoly();
            if (obsPoly.containsPoint(lonLat, Qt::OddEvenFill))
                noFlyViolations.insert(area);
        }
    }


    qDebug() << "max possible score" << maxScore;

    SimulatedFlierResults toRet;
    toRet.points = score;
    toRet.pointsPossible = maxScore;
    toRet.dependencyViolations = dependencyViolators;
    toRet.timingViolations = timingViolators;
    toRet.noFlyViolations = noFlyViolations;

    return toRet;
}
