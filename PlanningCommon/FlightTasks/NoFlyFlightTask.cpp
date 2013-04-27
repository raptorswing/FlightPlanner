#include "NoFlyFlightTask.h"

NoFlyFlightTask::NoFlyFlightTask()
{
}

//for de-serializing
NoFlyFlightTask::NoFlyFlightTask(QDataStream &stream) :
    FlightTask(stream)
{
}

//pure-virtual from Serializable
QString NoFlyFlightTask::serializationType() const
{
    return "NoFlyFlightTask";
}

//pure-virtual from Serializable
void NoFlyFlightTask::serialize(QDataStream &stream) const
{
    FlightTask::serialize(stream);
}

//virtual from FlightTask
bool NoFlyFlightTask::shortnessRewardApplies() const
{
    return false;
}

//pure-virtual from FlightTask
QString NoFlyFlightTask::taskType() const
{
    return "No-Fly Zone";
}

//pure-virtual from FlightTask
qreal NoFlyFlightTask::calculateFlightPerformance(const Wayset &wayset,
                                                  const QPolygonF &geoPoly,
                                                  const UAVParameters &,
                                                  bool includeEnticement,
                                                  qreal *progressStartOut,
                                                  qreal *progressEndOut)
{
    Q_UNUSED(includeEnticement)
    qreal fitness = this->maxTaskPerformance();

    const QList<Position> positions = wayset.positions();
    foreach(const Position& pos, positions)
    {
        if (geoPoly.containsPoint(pos.lonLat(), Qt::OddEvenFill))
        {
            fitness = 0.0;
            break;
        }
    }

    //This stuff is not defined for NoFly
    if (progressStartOut != 0)
        *progressStartOut = -1.0;

    if (progressEndOut != 0)
        *progressEndOut = -1.0;

    return fitness;
}
