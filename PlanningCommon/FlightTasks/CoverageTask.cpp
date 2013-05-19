#include "CoverageTask.h"

#include <QSet>

#include "guts/Conversions.h"

CoverageTask::CoverageTask(qreal coverageGranularity) :
    _granularity(coverageGranularity)
{
}

//for de-serializing
CoverageTask::CoverageTask(QDataStream &stream) :
    FlightTask(stream)
{
    stream >> _granularity;
}

//pure-virtual from Serializable
QString CoverageTask::serializationType() const
{
    return "CoverageTask";
}

//pure-virtual from Serializable
void CoverageTask::serialize(QDataStream &stream) const
{
    FlightTask::serialize(stream);
    stream << _granularity;
}

QString CoverageTask::taskType() const
{
    return "Coverage";
}

qreal CoverageTask::calculateFlightPerformance(const Wayset &wayset,
                                               const QPolygonF &geoPoly,
                                               const UAVParameters & params,
                                               bool includeEnticement,
                                               qreal *progressStartOut,
                                               qreal *progressEndOut)
{
    if (wayset.isEmpty())
        return 0.0;

    if (geoPoly != _lastGeoPoly || _bins.isEmpty())
        _calculateBins(geoPoly);

    int firstProgressIndex = -1;
    int lastProgressIndex = -1;


    QSet<int> satisfiedBins;
    for (int i = 0; i < wayset.size(); i++)
    {
        const UAVPose& pose = wayset.at(i);
        const Position& pos = pose.pos();

        for (int j = 0; j < _bins.size(); j++)
        {
            if (satisfiedBins.contains(j))
                continue;

            const qreal distance = pos.flatDistanceEstimate(_bins.at(j));

            if (distance < this->maxSensingDistance())
            {
                satisfiedBins.insert(j);

                //Record progress timing info...
                if (firstProgressIndex == -1)
                    firstProgressIndex = i;
                lastProgressIndex = i;
            }
        }
    }

    const qreal reward = satisfiedBins.size();

    qreal enticement = 0.0;
    for (int i = 0; i < _bins.size(); i++)
    {
        if (satisfiedBins.contains(i))
            continue;

        const qreal distance = wayset.last().pos().flatDistanceEstimate(_bins.at(i));
        const qreal currentEnticement = FlightTask::normal(distance, 200.0, 10.0);
        if (currentEnticement > enticement)
            enticement = currentEnticement;

        /*
         *Comment out this break if you want to use the nearest enticement point rather than
         *the one that is next in the list.
        */
        break;
    }

    if (progressStartOut != 0)
    {
        *progressStartOut = -1.0;
        if (firstProgressIndex != -1)
            *progressStartOut = wayset.distToPoseIndex(firstProgressIndex, params) / params.airspeed();
    }

    if (progressEndOut != 0)
    {
        *progressEndOut = -1.0;
        if (lastProgressIndex != -1)
            *progressEndOut = wayset.distToPoseIndex(lastProgressIndex, params) / params.airspeed();
    }

    if (includeEnticement)
        return reward + enticement;
    return reward;
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

    //Reset bins so that they are forced to recalculate next time
    _bins.clear();
    this->flightTaskChanged();
}

//private
void CoverageTask::_calculateBins(const QPolygonF &geoPoly)
{
    _bins.clear();
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
                _bins.append(lla);
        }
    }

    if (_bins.isEmpty())
        qWarning() << "Warning:" << this << "has empty bins";
}
