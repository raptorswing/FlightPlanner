#include "SamplingTask.h"

#include "guts/Conversions.h"

const qreal GRANULARITY = 75.0;

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
qreal SamplingTask::calculateFlightPerformance(const Wayset &wayset,
                                               const QPolygonF &geoPoly,
                                               const UAVParameters &uavParams,
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

    qreal toRet = 0.0;
    const QList<Position> positions = wayset.positions();
    for (int i = 0; i < wayset.size(); i++)
    {
        const Position& pos = positions.at(i);

        if (!geoPoly.containsPoint(pos.lonLat(), Qt::OddEvenFill))
            continue;

        if (firstProgressIndex == -1)
            firstProgressIndex = i;

        if (toRet < _timeRequired)
            lastProgressIndex = i;

        //Estimate the amount of time flown within the area, presumably sampling while doing so
        toRet += uavParams.waypointInterval() / uavParams.airspeed();
    }

    if (includeEnticement && !_bins.isEmpty())
    {
        qreal enticement = 0.0;

        const Position& toEntice = _bins.at(wayset.size() % _bins.size());
        const qreal distance = toEntice.flatDistanceEstimate(wayset.positions().last());
        enticement += FlightTask::normal(distance, 200.0, 10.0);

        toRet += enticement;
    }


    if (progressStartOut != 0 && firstProgressIndex != -1)
        *progressStartOut = wayset.distToPoseIndex(firstProgressIndex, uavParams) / uavParams.airspeed();

    if (progressEndOut != 0 && lastProgressIndex != -1)
        *progressEndOut = wayset.distToPoseIndex(lastProgressIndex, uavParams) / uavParams.airspeed();

    return qMin<qreal>(toRet, this->maxTaskPerformance());
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

//private
void SamplingTask::_calculateBins(const QPolygonF &geoPoly)
{
    _bins.clear();
    //_xyzBins.clear();
    _lastGeoPoly = geoPoly;

    const QRectF boundingRect = geoPoly.boundingRect().normalized();

    const QVector3D topLeftXYZ(Conversions::lla2xyz(boundingRect.topRight()));
    const QVector3D bottomLeftXYZ(Conversions::lla2xyz(boundingRect.bottomRight()));
    const QVector3D topRightXYZ(Conversions::lla2xyz(boundingRect.topLeft()));

    const qreal widthMeters = (topLeftXYZ - topRightXYZ).length();
    const qreal heightMeters = (topLeftXYZ - bottomLeftXYZ).length();
    const qreal lonPerMeter = Conversions::degreesLonPerMeter(boundingRect.center().y());
    const qreal latPerMeter = Conversions::degreesLatPerMeter(boundingRect.center().y());

    const int offset = GRANULARITY / 2;

    for (int x = 0; x < widthMeters / GRANULARITY; x++)
    {
        for (int y = 0; y < heightMeters / GRANULARITY; y++)
        {
            qreal lon = boundingRect.topLeft().x() + x * GRANULARITY * lonPerMeter;
            lon += offset * lonPerMeter;

            qreal lat = boundingRect.topLeft().y() + y * GRANULARITY * latPerMeter;
            lat += offset * latPerMeter;

            Position lla(lon, lat);
            if (geoPoly.containsPoint(lla.lonLat(), Qt::OddEvenFill))
                _bins.append(lla);
        }
    }

    if (_bins.isEmpty())
        qWarning() << "Warning:" << this << "has empty bins";
}
