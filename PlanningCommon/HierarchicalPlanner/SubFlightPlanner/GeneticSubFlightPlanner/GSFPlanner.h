#ifndef GSFPLANNER_H
#define GSFPLANNER_H

#include "HierarchicalPlanner/SubFlightPlanner/SubFlightPlanner.h"

class GSFPlanner : public SubFlightPlanner
{
public:
    GSFPlanner(const UAVParameters& uavParams,
               const QSharedPointer<FlightTask>& task,
               const QSharedPointer<FlightTaskArea>& area,
               const Position& startPos,
               const UAVOrientation& startPose);

    //virtual from SubFlightPlanner
    virtual bool plan();
};

#endif // GSFPLANNER_H
