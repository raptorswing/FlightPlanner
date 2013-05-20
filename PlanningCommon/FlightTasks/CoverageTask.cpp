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
                                               const UAVParameters & uavParams,
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

            const Position& binPos = _bins.at(j);
            const qreal distance = pos.flatDistanceEstimate(binPos);

            //Check minimum and maximum distance
            if (distance > this->maxSensingDistance() || distance < this->minSensingDistance())
                continue;

            //Check the angle from the bin position to the plane
            const qreal angleFromBin = binPos.angleTo(pos);
            if (this->minSensingDistance() > 0.0 && !this->validSensorAngleRange().withinRange(UAVOrientation(angleFromBin)))
                continue;

            //If directional sensor, check that UAV is facing the thing it is trying to sense
            if (this->sensorType() == DirectionalSensor)
            {
                AngleRange directionalAngleRange(UAVOrientation(0), uavParams.directionalSensorViewAngleRadians(),true);
                directionalAngleRange.setCenter(pose.angle());
                const qreal angleRads = pos.angleTo(binPos);
                if (!directionalAngleRange.withinRange(UAVOrientation(angleRads)))
                    continue;
            }

            satisfiedBins.insert(j);

            //Record progress timing info...
            if (firstProgressIndex == -1)
                firstProgressIndex = i;
            lastProgressIndex = i;
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
            *progressStartOut = wayset.distToPoseIndex(firstProgressIndex, uavParams) / uavParams.airspeed();
    }

    if (progressEndOut != 0)
    {
        *progressEndOut = -1.0;
        if (lastProgressIndex != -1)
            *progressEndOut = wayset.distToPoseIndex(lastProgressIndex, uavParams) / uavParams.airspeed();
    }

    if (includeEnticement)
        return reward + enticement;
    return reward;
}

const QList<Position> &CoverageTask::bins(const QPolygonF &geoPoly)
{
    if (_bins.isEmpty() || geoPoly != _lastGeoPoly)
        _calculateBins(geoPoly);

    return _bins;
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

//protected
//virtual from FlightTask
void CoverageTask::_calculateBins(const QPolygonF &geoPoly)
{
    _bins.clear();
    _lastGeoPoly = geoPoly;

    const QRectF boundingRect = geoPoly.boundingRect().normalized();

    const Position topLeft = boundingRect.topLeft();
    const Position bottomLeft = boundingRect.bottomLeft();
    const Position topRight = boundingRect.topRight();

    const qreal widthMeters = topLeft.flatDistanceEstimate(topRight);
    const qreal heightMeters = topLeft.flatDistanceEstimate(bottomLeft);

    const QPointF finalOffset = this->minSensingDistance()
            * QPointF(cos(this->validSensorAngleRange().center().radians()),
                      sin(this->validSensorAngleRange().center().radians()));

    for (int x = 0; x < widthMeters / _granularity; x++)
    {
        for (int y = 0; y < heightMeters / _granularity; y++)
        {
            const QPointF offset(x * _granularity + _granularity / 2.0,
                                 y * _granularity + _granularity / 2.0);

            const Position lla = topLeft.flatOffsetToPosition(offset);
            if (geoPoly.containsPoint(lla.lonLat(), Qt::OddEvenFill))
                _bins.append(lla.flatOffsetToPosition(finalOffset));
        }
    }

    if (_bins.isEmpty())
        qWarning() << "Warning:" << this << "has empty bins";
}
