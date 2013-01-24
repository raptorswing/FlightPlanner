#include "FlyThroughTask.h"

#include "guts/Conversions.h"

#include <QtDebug>

FlyThroughTask::FlyThroughTask()
{
}

QString FlyThroughTask::taskType() const
{
    return "Fly Through";
}

qreal FlyThroughTask::calculateFlightPerformance(const QList<Position> &positions,
                                                 const QPolygonF &geoPoly)
{
    //First, see if one of the points is within the polygon
    foreach(const Position& pos, positions)
    {
        if (geoPoly.containsPoint(pos.lonLat(), Qt::OddEvenFill))
            return this->maxTaskPerformance();
    }

    //if that fails, take the distance to the last point
    Position goalPos(geoPoly.boundingRect().center(),
                     positions.first().altitude());

    const Position& last = positions.last();
    QVector3D enuPos = Conversions::lla2enu(last, goalPos);
    qreal dist = enuPos.length();

    const qreal stdDev = 90.0;
    qreal toRet = 100*FlightTask::normal(dist,stdDev,2000);

    return qMin<qreal>(toRet,this->maxTaskPerformance());
}
