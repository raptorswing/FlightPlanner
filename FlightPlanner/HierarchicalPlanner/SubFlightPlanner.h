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
                     const Position& endPos,
                     const UAVOrientation& startPose,
                     const UAVOrientation& endPose);

    void plan();
    const QList<Position> &results() const;

private:
    void _greedyPlan();
    const QSharedPointer<FlightTask>& _task;
    const QSharedPointer<FlightTaskArea>& _area;
    const Position& _startPos;
    const Position& _endPos;
    const UAVOrientation& _startPose;
    const UAVOrientation& _endPose;

    QList<Position> _results;
};

#endif // SUBFLIGHTPLANNER_H
