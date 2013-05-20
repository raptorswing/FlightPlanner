#ifndef SUBFLIGHTPLANNER_H
#define SUBFLIGHTPLANNER_H

#include "UAVParameters.h"
#include "FlightTaskArea.h"
#include "FlightTasks/FlightTask.h"
#include "Position.h"

#include <QSharedPointer>

class SubFlightPlanner
{
public:
    SubFlightPlanner(const UAVParameters& uavParams,
                     const QSharedPointer<FlightTask>& task,
                     const QSharedPointer<FlightTaskArea>& area,
                     const Position& startPos,
                     const UAVOrientation& startPose);

    bool plan();
    const Wayset &results() const;

private:
    UAVParameters _uavParams;
    QSharedPointer<FlightTask> _task;
    QSharedPointer<FlightTaskArea> _area;
    Position _startPos;
    UAVOrientation _startPose;

    Wayset _toRet;
};

#endif // SUBFLIGHTPLANNER_H
