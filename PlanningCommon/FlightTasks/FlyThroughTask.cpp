#include "FlyThroughTask.h"

#include "guts/Conversions.h"

#include <QtDebug>

FlyThroughTask::FlyThroughTask()
{
}

//for de-serializing
FlyThroughTask::FlyThroughTask(QDataStream &stream) :
    FlightTask(stream)
{
}

//pure-virtual from Serializable
QString FlyThroughTask::serializationType() const
{
    return "FlyThroughTask";
}

//pure-virtual from Serializable
void FlyThroughTask::serialize(QDataStream &stream) const
{
    FlightTask::serialize(stream);
}

QString FlyThroughTask::taskType() const
{
    return "Fly Through";
}

qreal FlyThroughTask::calculateFlightPerformance(const Wayset &wayset,
                                                 const QPolygonF &geoPoly,
                                                 const UAVParameters & params,
                                                 bool includeEnticement,
                                                 qreal *progressStartOut,
                                                 qreal *progressEndOut)
{

    //First, see if one of the points is within the polygon
    int count = 0;
    const QList<Position> positions = wayset.positions();
    for (int i = 0; i < wayset.size(); i++)
    {
        const Position& pos = positions.at(i);

        if (geoPoly.containsPoint(pos.lonLat(), Qt::OddEvenFill))
        {
            if (includeEnticement && count++ < 5)
                continue;

            if (progressStartOut != 0)
                *progressStartOut = wayset.distToPoseIndex(i, params) / params.airspeed();
            if (progressEndOut != 0)
                *progressEndOut = wayset.distToPoseIndex(i, params) / params.airspeed();

            return this->maxTaskPerformance();
        }
    }

    //if that fails, take the distance to the last point
    const Position goalPos(geoPoly.boundingRect().center());

    const Position& last = wayset.last().pos();
    const qreal dist = goalPos.flatDistanceEstimate(last);

    const qreal stdDev = 90.0;
    qreal toRet = 10*FlightTask::normal(dist,stdDev,2000);

    if (includeEnticement)
        return qMin<qreal>(toRet,this->maxTaskPerformance());
    return 0.0;
}
