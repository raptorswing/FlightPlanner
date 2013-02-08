#include "CoverageTask.h"

#include <QSet>

#include "guts/Conversions.h"

CoverageTask::CoverageTask(qreal coverageGranularity, qreal maxSatisfyingDistance) :
    _granularity(coverageGranularity), _maxDistance(maxSatisfyingDistance)
{
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

    const qreal maxDistance = _maxDistance;

    QSet<int> satisfiedBins;

    foreach(const Position& pos, positions)
    {
        const QVector3D xyz = Conversions::lla2xyz(pos);
        for (int i = 0; i < _bins.size(); i++)
        {
            if (satisfiedBins.contains(i))
                continue;
            const qreal distance = (xyz - _xyzBins.value(i)).length();
            if (distance < maxDistance)
                satisfiedBins.insert(i);
        }
    }

    const qreal reward = satisfiedBins.size();

    qreal enticement = 0.0;
    const QVector3D lastPosXYZ = Conversions::lla2xyz(positions.last());
    for (int i = 0; i < _bins.size(); i++)
    {
        if (satisfiedBins.contains(i))
            continue;

        const qreal distance = (lastPosXYZ - _xyzBins.value(i)).length();
        enticement = FlightTask::normal(distance, 200.0, 10.0);
        break;
    }

    return reward + enticement;
}

qreal CoverageTask::maxTaskPerformance() const
{
    if (_bins.size() == 0)
        return FlightTask::maxTaskPerformance();
    return (qreal) _bins.size();
}

qreal CoverageTask::granularity() const
{
    return _granularity;
}

void CoverageTask::setGranularity(qreal nGran)
{
    _granularity = qBound<qreal>(1.0, nGran, 1000.0);
    this->flightTaskChanged();
}

qreal CoverageTask::maxDistance() const
{
    return _maxDistance;
}

void CoverageTask::setMaxDistance(qreal maxDist)
{
    _maxDistance = qBound<qreal>(1.0, maxDist, 1000.0);
    this->flightTaskChanged();
}

//private
void CoverageTask::_calculateBins(const QPolygonF &geoPoly)
{
    _bins.clear();
    _xyzBins.clear();
    _lastGeoPoly = geoPoly;

    const QRectF boundingRect = geoPoly.boundingRect().normalized();

    const QVector3D topLeftXYZ(Conversions::lla2xyz(boundingRect.topRight()));
    const QVector3D bottomLeftXYZ(Conversions::lla2xyz(boundingRect.bottomRight()));
    const QVector3D topRightXYZ(Conversions::lla2xyz(boundingRect.topLeft()));

    const qreal widthMeters = (topLeftXYZ - topRightXYZ).length();
    const qreal heightMeters = (topLeftXYZ - bottomLeftXYZ).length();
    const qreal lonPerMeter = Conversions::degreesLonPerMeter(boundingRect.center().y());
    const qreal latPerMeter = Conversions::degreesLatPerMeter(boundingRect.center().y());

    const int offset = _granularity / 2;

    for (int x = 0; x < widthMeters / _granularity; x++)
    {
        for (int y = 0; y < heightMeters / _granularity; y++)
        {
            qreal lon = boundingRect.topLeft().x() + x * _granularity * lonPerMeter;
            lon += offset * lonPerMeter;

            qreal lat = boundingRect.topLeft().y() + y * _granularity * latPerMeter;
            lat += offset * latPerMeter;

            Position lla(lon, lat);
            if (geoPoly.containsPoint(lla.lonLat(), Qt::OddEvenFill))
            {
                _bins.append(lla);
                _xyzBins.append(Conversions::lla2xyz(lla));
            }
        }
    }
}
