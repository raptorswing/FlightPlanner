#ifndef DUBINSUBFLIGHTPLANNER_H
#define DUBINSUBFLIGHTPLANNER_H

#include "UAVParameters.h"
#include "FlightTaskArea.h"
#include "FlightTasks/FlightTask.h"
#include "Position.h"
#include "SubFlightPlanner.h"

#include <QSharedPointer>

class DubinSubFlightPlanner : public SubFlightPlanner
{
public:
    DubinSubFlightPlanner(const UAVParameters& uavParams,
                          const QSharedPointer<FlightTask>& task,
                          const QSharedPointer<FlightTaskArea>& area,
                          const Position& startPos,
                          const UAVOrientation& startPose);

    //virtual from SubFlightPlanner
    virtual bool plan();

private:
};

#endif // DUBINSUBFLIGHTPLANNER_H
