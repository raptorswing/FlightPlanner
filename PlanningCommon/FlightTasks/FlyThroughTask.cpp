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

qreal FlyThroughTask::calculateFlightPerformance(const Wayset &positions,
                                                 const QPolygonF &geoPoly,
                                                 const UAVParameters & params,
                                                 bool includeEnticement,
                                                 qreal *progressStartOut,
                                                 qreal *progressEndOut)
{

    //First, see if one of the points is within the polygon
    //foreach(const Position& pos, positions.positions())
    for (int i = 0; i < positions.size(); i++)
    {
        const Position& pos = positions.positions().at(i);

        if (geoPoly.containsPoint(pos.lonLat(), Qt::OddEvenFill))
        {
            if (progressStartOut != 0)
                *progressStartOut = positions.distToPoseIndex(i, params) / params.airspeed();
            if (progressEndOut != 0)
                *progressEndOut = positions.distToPoseIndex(i, params) / params.airspeed();

            return this->maxTaskPerformance();
        }
    }

    //if that fails, take the distance to the last point
    Position goalPos(geoPoly.boundingRect().center(),
                     positions.first().pos().altitude());

    const Position& last = positions.last().pos();
    QVector3D enuPos = Conversions::lla2enu(last, goalPos);
    qreal dist = enuPos.length();

    const qreal stdDev = 90.0;
    qreal toRet = 100*FlightTask::normal(dist,stdDev,2000);

    if (includeEnticement)
        return qMin<qreal>(toRet,this->maxTaskPerformance());
    return 0.0;
}
