#ifndef SUBFLIGHTPLANNER_H
#define SUBFLIGHTPLANNER_H

#include <QSharedPointer>
#include <QList>

#include "FlightTaskArea.h"
#include "FlightTasks/FlightTask.h"
#include "Position.h"
#include "UAVOrientation.h"

class SubFlightPlanner
{
public:
    SubFlightPlanner(const QSharedPointer<FlightTask>& task,
                     const QSharedPointer<FlightTaskArea>& area,
                     const Position& startPos,
                     const UAVOrientation& startPose);

    void plan();
    const QList<Position> &results() const;

private:
    void _greedyPlan();
    const QSharedPointer<FlightTask>& _task;
    const QSharedPointer<FlightTaskArea>& _area;
    const Position& _startPos;
    const UAVOrientation& _startPose;

    QList<Position> _results;
};

#endif // SUBFLIGHTPLANNER_H
