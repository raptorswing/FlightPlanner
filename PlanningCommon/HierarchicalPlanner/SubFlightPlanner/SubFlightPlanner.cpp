#include "SubFlightPlanner.h"

#include "GreedySubFlightPlanner.h"
#include "DubinSubFlightPlanner.h"

SubFlightPlanner::SubFlightPlanner(const UAVParameters &uavParams,
                                   const QSharedPointer<FlightTask> &task,
                                   const QSharedPointer<FlightTaskArea> &area,
                                   const Position &startPos,
                                   const UAVOrientation &startPose) :
    _uavParams(uavParams), _task(task), _area(area), _startPos(startPos), _startPose(startPose)
{
}

bool SubFlightPlanner::plan()
{
    _toRet.clear();

    if (_task->sensorType() == FlightTask::OmnidirectionalSensor)
    {
        GreedySubFlightPlanner planner(_uavParams,
                                       _task,
                                       _area,
                                       _startPos,
                                       _startPose);
        if (!planner.plan())
            return false;

        _toRet = planner.results();
    }
    else if (_task->sensorType() == FlightTask::DirectionalSensor)
    {
        DubinSubFlightPlanner planner(_uavParams,
                                      _task,
                                      _area,
                                      _startPos,
                                      _startPose);
        if (!planner.plan())
            return false;

        _toRet = planner.results();
    }

    return true;
}

const Wayset &SubFlightPlanner::results() const
{
    return _toRet;
}
