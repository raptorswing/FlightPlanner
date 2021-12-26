#ifndef SUBFLIGHTPLANNER_H
#define SUBFLIGHTPLANNER_H

#include "UAVParameters.h"
#include "FlightTaskArea.h"
#include "FlightTasks/FlightTask.h"
#include "Position.h"

#include <QSharedPointer>

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT SubFlightPlanner
{
public:
    SubFlightPlanner(const UAVParameters& uavParams,
                     const QSharedPointer<FlightTask>& task,
                     const QSharedPointer<FlightTaskArea>& area,
                     const Position& startPos,
                     const UAVOrientation& startPose);
    virtual ~SubFlightPlanner();

    virtual bool plan();
    const Wayset &results() const;

protected:
    const UAVParameters& uavParams() const;
    const QSharedPointer<FlightTask>& task() const;
    const QSharedPointer<FlightTaskArea>& area() const;
    const Position& startPos() const;
    const UAVOrientation& startPose() const;

    void setResults(const Wayset& results);

private:
    UAVParameters _uavParams;
    QSharedPointer<FlightTask> _task;
    QSharedPointer<FlightTaskArea> _area;
    Position _startPos;
    UAVOrientation _startPose;

    Wayset _toRet;
};

#endif // SUBFLIGHTPLANNER_H
