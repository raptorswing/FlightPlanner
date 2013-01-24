#include "NoFlyFlightTask.h"

NoFlyFlightTask::NoFlyFlightTask()
{
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
qreal NoFlyFlightTask::calculateFlightPerformance(const QList<Position> &positions, const QPolygonF &geoPoly)
{
    qreal fitness = this->maxTaskPerformance();

    foreach(const Position& pos, positions)
    {
        if (geoPoly.containsPoint(pos.lonLat(), Qt::OddEvenFill))
        {
            fitness = 0.0;
            break;
        }
    }

    return fitness;
}
