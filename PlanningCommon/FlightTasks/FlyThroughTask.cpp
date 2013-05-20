#include "FlyThroughTask.h"

#include "guts/Conversions.h"

#include <QtDebug>
#include <limits>

const qreal GRANULARITY = 10.0; //meters

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
    if (_lastGeoPoly != geoPoly || _bins.isEmpty())
        _calculateBins(geoPoly);

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
    Position goalPos(geoPoly.boundingRect().center());
    if (!_bins.isEmpty())
        goalPos = _bins.first();

    const Position& last = wayset.last().pos();
    const qreal dist = goalPos.flatDistanceEstimate(last);

    const qreal stdDev = 90.0;
    qreal toRet = 10*FlightTask::normal(dist,stdDev,2000);

    if (includeEnticement)
        return qMin<qreal>(toRet,this->maxTaskPerformance());
    return 0.0;
}

const QList<Position> &FlyThroughTask::bins(const QPolygonF &geoPoly)
{
    if (_lastGeoPoly != geoPoly || _bins.isEmpty())
        _calculateBins(geoPoly);

    return _bins;
}

//protected
//virtual from FlightTask
void FlyThroughTask::_calculateBins(const QPolygonF &geoPoly)
{
    _bins.clear();
    _lastGeoPoly = geoPoly;

    const QRectF boundingRect = geoPoly.boundingRect().normalized();

    const Position topLeft = boundingRect.topLeft();
    const Position bottomLeft = boundingRect.bottomLeft();
    const Position topRight = boundingRect.topRight();

    const qreal widthMeters = topLeft.flatDistanceEstimate(topRight);
    const qreal heightMeters = topLeft.flatDistanceEstimate(bottomLeft);

    const Position centerPos(geoPoly.boundingRect().center());

    Position best;
    qreal bestDist = std::numeric_limits<qreal>::max();

    for (int x = 0; x < widthMeters / GRANULARITY; x++)
    {
        for (int y = 0; y < heightMeters / GRANULARITY; y++)
        {
            const QPointF offset(x * GRANULARITY,
                                  y * GRANULARITY);

            const Position lla = topLeft.flatOffsetToPosition(offset);
            if (!geoPoly.containsPoint(lla.lonLat(), Qt::OddEvenFill))
                continue;

            const qreal dist = lla.flatDistanceEstimate(centerPos);
            if (dist < bestDist)
            {
                bestDist = dist;
                best = lla;
            }
        }
    }
    _bins.append(best);

    if (_bins.isEmpty())
        qWarning() << "Warning:" << this << "has empty bins";
}
