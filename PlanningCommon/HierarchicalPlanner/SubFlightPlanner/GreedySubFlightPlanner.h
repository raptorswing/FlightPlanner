#ifndef GreedySubFlightPlanner_H
#define GreedySubFlightPlanner_H

#include <QSharedPointer>
#include <QList>

#include "SubFlightPlanner.h"
#include "FlightTaskArea.h"
#include "FlightTasks/FlightTask.h"
#include "Position.h"
#include "UAVOrientation.h"
#include "UAVParameters.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT GreedySubFlightPlanner : public SubFlightPlanner
{
public:
    GreedySubFlightPlanner(const UAVParameters& uavParams,
                           const QSharedPointer<FlightTask>& task,
                           const QSharedPointer<FlightTaskArea>& area,
                           const Position& startPos,
                           const UAVOrientation& startPose);

    //virtual from SubFlightPlanner
    virtual bool plan();

private:
    bool _greedyPlan();
};

#endif // GreedySubFlightPlanner_H
