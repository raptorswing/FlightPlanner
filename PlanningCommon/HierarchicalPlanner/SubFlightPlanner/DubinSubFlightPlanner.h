#ifndef DUBINSUBFLIGHTPLANNER_H
#define DUBINSUBFLIGHTPLANNER_H

#include "UAVParameters.h"
#include "FlightTaskArea.h"
#include "FlightTasks/FlightTask.h"
#include "Position.h"

#include <QSharedPointer>

class DubinSubFlightPlanner
{
public:
    DubinSubFlightPlanner(const UAVParameters& uavParams,
                          const QSharedPointer<FlightTask>& task,
                          const QSharedPointer<FlightTaskArea>& area,
                          const Position& startPos,
                          const UAVOrientation& startPose);

    bool plan();
    const Wayset &results() const;

private:
    const UAVParameters& _uavParams;
    const QSharedPointer<FlightTask>& _task;
    const QSharedPointer<FlightTaskArea>& _area;
    const Position& _startPos;
    const UAVOrientation& _startPose;

    Wayset _toRet;
};

#endif // DUBINSUBFLIGHTPLANNER_H
