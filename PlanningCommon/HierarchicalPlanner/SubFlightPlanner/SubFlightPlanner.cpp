#include "SubFlightPlanner.h"

#include "GreedySubFlightPlanner/GreedySubFlightPlanner.h"
#include "DubinSubFlightPlanner/DubinSubFlightPlanner.h"
#include "GeneticSubFlightPlanner/GSFPlanner.h"

SubFlightPlanner::SubFlightPlanner(const UAVParameters &uavParams,
                                   const QSharedPointer<FlightTask> &task,
                                   const QSharedPointer<FlightTaskArea> &area,
                                   const Position &startPos,
                                   const UAVOrientation &startPose) :
    _uavParams(uavParams), _task(task), _area(area), _startPos(startPos), _startPose(startPose)
{
}

SubFlightPlanner::~SubFlightPlanner()
{
}

bool SubFlightPlanner::plan()
{
    _toRet.clear();

    //    GSFPlanner planner(this->uavParams(),
    //                       this->task(),
    //                       this->area(),
    //                       this->startPos(),
    //                       this->startPose());
    //    if (!planner.plan())
    //        return false;
    //    this->setResults(planner.results());
    //    return true;

    DubinSubFlightPlanner planner(_uavParams,
                                  _task,
                                  _area,
                                  _startPos,
                                  _startPose);
    if (!planner.plan())
        return false;

    _toRet = planner.results();

    return true;
}

const Wayset &SubFlightPlanner::results() const
{
    return _toRet;
}

//protected
const UAVParameters &SubFlightPlanner::uavParams() const
{
    return _uavParams;
}

//protected
const QSharedPointer<FlightTask> &SubFlightPlanner::task() const
{
    return _task;
}

//protected
const QSharedPointer<FlightTaskArea> &SubFlightPlanner::area() const
{
    return _area;
}

//protected
const Position &SubFlightPlanner::startPos() const
{
    return _startPos;
}

//protected
const UAVOrientation &SubFlightPlanner::startPose() const
{
    return _startPose;
}

//protected
void SubFlightPlanner::setResults(const Wayset &results)
{
    _toRet = results;
}
