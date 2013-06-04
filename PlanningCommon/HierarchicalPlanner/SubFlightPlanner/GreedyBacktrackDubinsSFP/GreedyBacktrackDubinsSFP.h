#ifndef GREEDYBACKTRACKDUBINSSFP_H
#define GREEDYBACKTRACKDUBINSSFP_H

#include "HierarchicalPlanner/SubFlightPlanner/SubFlightPlanner.h"

class GreedyBacktrackDubinsSFP : public SubFlightPlanner
{
public:
    GreedyBacktrackDubinsSFP(const UAVParameters &uavParams,
                             const QSharedPointer<FlightTask> &task,
                             const QSharedPointer<FlightTaskArea> &area,
                             const Position &startPos,
                             const UAVOrientation &startPose);

    //virtual from SubFlightPlanner
    virtual bool plan();
};

#endif // GREEDYBACKTRACKDUBINSSFP_H
