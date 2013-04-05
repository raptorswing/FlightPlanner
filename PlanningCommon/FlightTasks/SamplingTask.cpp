#include "SamplingTask.h"

SamplingTask::SamplingTask(const qreal timeRequired) : _timeRequired(timeRequired)
{
}

//for de-serializing
SamplingTask::SamplingTask(QDataStream &stream) :
    FlightTask(stream)
{
    stream >> _timeRequired;
}

//pure-virtual from Serializable
QString SamplingTask::serializationType() const
{
    return "SamplingTask";
}

//pure-virtual from Serializable
void SamplingTask::serialize(QDataStream &stream) const
{
    FlightTask::serialize(stream);
    stream << _timeRequired;
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
qreal SamplingTask::calculateFlightPerformance(const Wayset &positions,
                                               const QPolygonF &geoPoly,
                                               const UAVParameters &uavParams,
                                               bool includeEnticement)
{
    Q_UNUSED(includeEnticement)
    qreal toRet = 0.0;

    const Wayset resampled = positions.resample(uavParams.waypointInterval(),
                                                uavParams);

    foreach(const Position& pos, resampled.positions())
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
