#include "CoverageTask.h"

#include <QSet>

#include "guts/Conversions.h"

const qreal GRANULARITY = 100.0;

CoverageTask::CoverageTask()
{
    _debug = false;
}

QString CoverageTask::taskType() const
{
    return "Coverage";
}

qreal CoverageTask::calculateFlightPerformance(const QList<Position> &positions, const QPolygonF &geoPoly)
{
    if (positions.isEmpty())
        return 0.0;

    if (geoPoly != _lastGeoPoly)
        _calculateBins(geoPoly);

    const qreal maxDistance = 100.0;

    QSet<int> satisfiedBins;

    foreach(const Position& pos, positions)
    {
        if (_debug)
            qDebug() << pos;
        const QVector3D xyz = Conversions::lla2xyz(pos);
        for (int i = 0; i < _bins.size(); i++)
        {
            if (satisfiedBins.contains(i))
                continue;
            const qreal distance = (xyz - _xyzBins.value(i)).length();
            if (distance < maxDistance)
                satisfiedBins.insert(i);

            if (_debug)
                qDebug() << i << "distance to" << _bins[i] << distance;
        }
    }

    if (_debug)
    {
        QList<int> satisfied = satisfiedBins.toList();
        foreach(int i, satisfied)
            qDebug() << i << "satisfied";
    }

    const qreal reward = satisfiedBins.size();

    qreal enticement = 0.0;
    const QVector3D lastPosXYZ = Conversions::lla2xyz(positions.last());
    for (int i = 0; i < _bins.size(); i++)
    {
        if (satisfiedBins.contains(i))
            continue;

        const qreal distance = (lastPosXYZ - _xyzBins.value(i)).length();
        //enticement = (1.0 - distance / 10000.0) * 0.5;
        enticement = FlightTask::normal(distance, 200.0, 10.0);
        qDebug() << distance << QString::number(enticement,'g',10);
        break;
    }

    return reward + enticement;
}

qreal CoverageTask::maxTaskPerformance() const
{
    return (qreal) _bins.size();
}

//private
void CoverageTask::_calculateBins(const QPolygonF &geoPoly)
{
    _bins.clear();
    _xyzBins.clear();
    _lastGeoPoly = geoPoly;

    const QRectF boundingRect = geoPoly.boundingRect();

    const Position topLeftLLA(boundingRect.topLeft());
    const QVector3D topLeftENU(0,0,0);
    const QVector3D topRightENU(Position::Position2ENU(Position(boundingRect.topRight()), topLeftLLA));
    const QVector3D bottomLeftENU(Position::Position2ENU(Position(boundingRect.bottomLeft()), topLeftLLA));

    qreal widthMeters = (topLeftENU - topRightENU).length();
    qreal heightMeters = (topLeftENU - bottomLeftENU).length();

    const int offset = 0;

    for (int x = offset; x < widthMeters / GRANULARITY - offset; x++)
    {
        for (int y = offset; y < heightMeters / GRANULARITY - offset; y++)
        {
            QVector3D enu(topLeftENU.x() + x * GRANULARITY,
                          topLeftENU.y() + y * GRANULARITY,
                          0.0);

            Position lla = Position::fromENU(topLeftLLA, enu);
            if (geoPoly.containsPoint(lla.lonLat(), Qt::OddEvenFill))
            {
                _bins.append(lla);
                _xyzBins.append(Conversions::lla2xyz(lla));
            }
        }
    }
}
