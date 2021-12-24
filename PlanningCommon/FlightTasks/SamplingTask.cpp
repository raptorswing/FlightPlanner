#include "SamplingTask.h"

#include "guts/Conversions.h"

#include <cmath>

const qreal GRANULARITY = 50.0;

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

    UAVParameters fudgeParams = uavParams;
    fudgeParams.setMinTurningRadius(fudgeParams.minTurningRadius() * 1.02);
    fudgeParams.setWaypointInterval(10.0);
    Wayset resampled = wayset.resample(fudgeParams.waypointInterval(), fudgeParams);

    if (geoPoly != _lastGeoPoly || _bins.isEmpty())
        _calculateBins(geoPoly);

    int firstProgressIndex = -1;
    int lastProgressIndex = -1;

    qreal toRet = 0.0;
    const QList<UAVPose> poses = resampled.poses();
    for (int i = 0; i < resampled.size(); i++)
    {
        const UAVPose& pose = poses.at(i);
        const Position& pos = pose.pos();

        /*
         *We are said to be making progress if we are within the proper distance range of one of
         *our bins and (for directional sensor) we are facing it
        */
        bool nearABin = false;
        foreach(const Position& binPos, this->bins(geoPoly))
        {
            const qreal dist = binPos.flatDistanceEstimate(pos);
            if (dist < this->minSensingDistance() || dist > this->maxSensingDistance())
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

            //If our checks pass, then we are making progress
            nearABin = true;
            break;
        }

        if (!nearABin)
            continue;

        if (firstProgressIndex == -1)
            firstProgressIndex = i;

        if (toRet < _timeRequired)
            lastProgressIndex = i;

        //Estimate the amount of time flown within the area, presumably sampling while doing so
        toRet += fudgeParams.waypointInterval() / uavParams.airspeed();
    }

    if (includeEnticement && !_transformedBins.isEmpty())
    {
        qreal enticement = 0.0;

        const Position& toEntice = _transformedBins.at(resampled.size() % _transformedBins.size());
        const qreal distance = toEntice.flatDistanceEstimate(poses.last().pos());
        enticement += FlightTask::normal(distance, 200.0, 10.0);

        toRet += enticement;
    }

    if (progressStartOut != 0 && firstProgressIndex != -1)
        *progressStartOut = resampled.distToPoseIndex(firstProgressIndex, uavParams) / uavParams.airspeed();

    if (progressEndOut != 0 && lastProgressIndex != -1)
        *progressEndOut = resampled.distToPoseIndex(lastProgressIndex, uavParams) / uavParams.airspeed();

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

//protected
//virtual from FlightTask
void SamplingTask::_calculateBins(const QPolygonF &geoPoly)
{
    _clearBins();
    _lastGeoPoly = geoPoly;

    const QRectF boundingRect = geoPoly.boundingRect().normalized();

    const Position topLeft = boundingRect.topLeft();
    const Position bottomLeft = boundingRect.bottomLeft();
    const Position topRight = boundingRect.topRight();

    const qreal widthMeters = topLeft.flatDistanceEstimate(topRight);
    const qreal heightMeters = topLeft.flatDistanceEstimate(bottomLeft);

    const qreal avgSensingDistance = (this->minSensingDistance() + this->maxSensingDistance()) / 2.0;
    const QPointF finalOffset = avgSensingDistance
            * QPointF(::cos(this->validSensorAngleRange().center().radians()),
                      ::sin(this->validSensorAngleRange().center().radians()));

    for (int x = 0; x < widthMeters / GRANULARITY; x++)
    {
        for (int y = 0; y < heightMeters / GRANULARITY; y++)
        {
            const QPointF offset(x * GRANULARITY + GRANULARITY / 2.0,
                                 y * GRANULARITY + GRANULARITY / 2.0);

            const Position lla = topLeft.flatOffsetToPosition(offset);
            if (!geoPoly.containsPoint(lla.lonLat(), Qt::OddEvenFill))
                continue;
            _bins.append(lla);
            _transformedBins.append(lla.flatOffsetToPosition(finalOffset));
        }
    }

    if (_bins.isEmpty())
        qWarning() << "Warning:" << this << "has empty bins";
}
