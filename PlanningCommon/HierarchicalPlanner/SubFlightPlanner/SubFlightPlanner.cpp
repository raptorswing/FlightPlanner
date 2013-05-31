#include "SubFlightPlanner.h"

#include "GreedySubFlightPlanner/GreedySubFlightPlanner.h"
#include "DubinSubFlightPlanner/DubinSubFlightPlanner.h"
#include "GeneticSubFlightPlanner/GSFPlanner.h"

#include <QSharedPointer>

const int WHICH_PLANNER = 1;

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

    QSharedPointer<SubFlightPlanner> planner;

    if (WHICH_PLANNER == 0)
    {
    planner = QSharedPointer<SubFlightPlanner>(new GSFPlanner(this->uavParams(),
                                                              this->task(),
                                                              this->area(),
                                                              this->startPos(),
                                                              this->startPose()));
    }
    else if (WHICH_PLANNER == 1)
    {
        planner = QSharedPointer<SubFlightPlanner>(new DubinSubFlightPlanner(this->uavParams(),
                                                                             this->task(),
                                                                             this->area(),
                                                                             this->startPos(),
                                                                             this->startPose()));
    }
    else if (WHICH_PLANNER == 2)
    {
        planner = QSharedPointer<SubFlightPlanner>(new GreedySubFlightPlanner(this->uavParams(),
                                                                              this->task(),
                                                                              this->area(),
                                                                              this->startPos(),
                                                                              this->startPose()));
    }
    else
    {
        qWarning() << "Warning: No sub-flight planner selected";
        return false;
    }

    if (!planner->plan())
        return false;
    this->setResults(planner->results());

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
