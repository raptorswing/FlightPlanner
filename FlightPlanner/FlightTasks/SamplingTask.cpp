#include "SamplingTask.h"

SamplingTask::SamplingTask(const qreal timeRequired) : _timeRequired(timeRequired)
{
}

//virtual from FlightTask
bool SamplingTask::shortnessRewardApplies() const
{
    return true;
}

//virtual from FlightTask
QString SamplingTask::taskType() const
{
    return "Sampling";
}

//virtual from FlightTask
qreal SamplingTask::calculateFlightPerformance(const QList<Position> &positions,
                                               const QPolygonF &geoPoly,
                                               const UAVParameters &uavParams)
{
    qreal toRet = 0.0;

    foreach(const Position& pos, positions)
    {
        if (!geoPoly.containsPoint(pos.lonLat(), Qt::OddEvenFill))
            continue;

        //Estimate the amount of time flown within the area, presumably sampling while doing so
        toRet += uavParams.waypointInterval() / uavParams.airspeed();
    }

    return toRet;
}

//virtual from FlightTask
qreal SamplingTask::maxTaskPerformance() const
{
    return _timeRequired;
}

qreal SamplingTask::timeRequired() const
{
    return _timeRequired;
}

void SamplingTask::setTimeRequired(qreal nTime)
{
    _timeRequired = nTime;
    this->flightTaskChanged();
}
