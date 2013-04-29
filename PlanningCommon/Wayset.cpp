#include "Wayset.h"

#include <QtDebug>
#include <QtCore>
#include <QVector2D>
#include <QMutableListIterator>

const qreal EPSILON_METERS = 0.001;

Wayset::Wayset()
{
}

Wayset::Wayset(const QList<UAVPose> &waypoints)
{
    foreach(const UAVPose& pose, waypoints)
        this->append(pose);
}

Wayset::Wayset(QDataStream &stream)
{
    int numPoses;
    stream >> numPoses;

    for (int i = 0; i < numPoses; i++)
    {
        Position pos;
        stream >> pos;

        UAVOrientation angle(stream);

        this->append(pos, angle);
    }
}

const UAVPose &Wayset::at(int i) const
{
    if (i < 0 || i >= this->size())
        qWarning() << "Index" << i << "out of bounds!";
    return _poses.at(i);
}

const UAVPose &Wayset::first() const
{
    return _poses.first();
}

const UAVPose &Wayset::last() const
{
    return _poses.last();
}

int Wayset::count() const
{
    return _poses.size();
}

int Wayset::size() const
{
    return _poses.size();
}

bool Wayset::isEmpty() const
{
    return _poses.isEmpty();
}

qreal Wayset::lengthMeters(const UAVParameters &uavParams) const
{
    qreal toRet = 0.0;
    const QList<Dubins> dubins = this->dubins(uavParams);

    foreach(const Dubins& dubin, dubins)
        toRet += dubin.length();

    return toRet;
}

QList<qreal> Wayset::waypointSubLengthsMeters(const UAVParameters &uavParams) const
{
    QList<qreal> toRet;

    const QList<Dubins> dubins = this->dubins(uavParams);

    qreal total = 0.0;
    foreach(const Dubins& d, dubins)
    {
        toRet.append(total);
        total += d.length();
    }

    //Plus the last one
    toRet.append(total);

    return toRet;
}

qreal Wayset::timeToFly(const UAVParameters &uavParams) const
{
    const qreal meters = this->lengthMeters(uavParams);
    const qreal ttf = meters / uavParams.airspeed();
    return ttf;
}

Wayset Wayset::resample(qreal granularityMeters,
                        const UAVParameters &uavParams) const
{
    Wayset toRet;

    const qreal totalLengthMeters = this->lengthMeters(uavParams);
    const int numSamples = qFloor(totalLengthMeters / granularityMeters);

    for (int i = 0; i < numSamples; i++)
    {
        const qreal desiredDist = granularityMeters * i;
        const UAVPose pose = this->sampleAtDistance(desiredDist, uavParams);
        toRet.append(pose);
    }
    if (toRet.last() != this->last())
        toRet.append(this->last());

    return toRet;
}

QList<Dubins> Wayset::dubins(const UAVParameters& uavParams) const
{
    QList<Dubins> toRet;

    for (int i = 0; i < this->count() - 1; i++)
    {
        const Position& a = this->at(i).pos();
        const Position& b = this->at(i + 1).pos();

        const QVector2D startOffset(0.0, 0.0);
        const QVector2D endOffset(a.flatOffsetMeters(b));

        const qreal startAngle = this->poses().at(i).angle().radians();
        const qreal endAngle = this->poses().at(i + 1).angle().radians();

        Dubins dubins(startOffset, startAngle, endOffset, endAngle, uavParams.minTurningRadius());
        if (!dubins.isValid())
        {
            qWarning() << "Invalid dubins in wayset!";
            continue;
        }
        toRet.append(dubins);
    }

    return toRet;
}

void Wayset::optimizeAngles(const UAVParameters &uavParams)
{
    qDebug() << "Optimize wayset angles";
    int count = 0;
    bool changes = true;
    while (changes)
    {
        changes = false;
        for (int i = 0; i < this->size(); i++)
        {
            const UAVOrientation& currentAngle = this->poses().at(i).angle();
            const qreal currentLength = this->lengthMeters(uavParams);

            const UAVOrientation lessOneAngle(currentAngle.degrees() - 1.0, false);
            _poses[i].setAngle(lessOneAngle);
            const qreal lessOneLength = this->lengthMeters(uavParams);

            const UAVOrientation addOneAngle(currentAngle.degrees() + 1.0, false);
            _poses[i].setAngle(addOneAngle);
            const qreal addOneLength = this->lengthMeters(uavParams);

            _poses[i].setAngle(currentAngle);

            qreal shortestLength = currentLength;
            if (lessOneLength < shortestLength)
            {
                _poses[i].setAngle(lessOneAngle);
                shortestLength = lessOneLength;
                changes = true;
            }
            if (addOneLength < shortestLength)
            {
                _poses[i].setAngle(addOneAngle);
                shortestLength = addOneLength;
                changes = true;
            }
        }
        count++;
    }
    qDebug() << "Optimized in" << count << "iterations";
}

UAVPose Wayset::sampleAtTime(qreal desiredTime, const UAVParameters &uavParams) const
{
    const qreal desiredDist = desiredTime * uavParams.airspeed();
    return this->sampleAtDistance(desiredDist, uavParams);
}

UAVPose Wayset::sampleAtDistance(qreal desiredDistance, const UAVParameters& uavParams) const
{
    qreal maxDist = 0.0;
    const QList<Dubins> dubins = this->dubins(uavParams);

    foreach(const Dubins& dubin, dubins)
        maxDist += dubin.length();

    if (desiredDistance < 0.0 || desiredDistance - maxDist > EPSILON_METERS)
    {
        qWarning() << "sampleAtDistance beyond valid distance window." << desiredDistance << "(max" << maxDist << ")";
        qDebug() << QString::number(desiredDistance, 'g', 20) << QString::number(maxDist, 'g', 20);
        return UAVPose();
    }
    else if (qFuzzyCompare(desiredDistance + 1.0, maxDist + 1.0))
        return this->last();

    //The total distance of all segments traversed so far
    qreal distSoFar = 0.0;

    UAVPose toRet;

    for (int i = 0; i < _poses.size() - 1; i++)
    {
        //The next segment of the path
        const Dubins& dubin = dubins.at(i);

        //How long is this segment of the path?
        const qreal distAddition = dubin.length();

        if (distSoFar + distAddition > desiredDistance)
        {
            const qreal subDistToSample = desiredDistance - distSoFar;

            QPointF offsetMeters;
            qreal angle;
            dubin.sample(subDistToSample, offsetMeters, angle);

            const UAVPose& offsetBasePose = _poses.at(i);
            const Position& offsetBase = offsetBasePose.pos();
            const Position newPosition = offsetBase.flatOffsetToPosition(offsetMeters);

            toRet = UAVPose(newPosition, angle);
            break;
        }

        distSoFar += distAddition;
    }

    return toRet;
}

qreal Wayset::distToPose(const UAVPose &pose, const UAVParameters &uavParams) const
{
    int indexOf = _poses.indexOf(pose);

    if (indexOf < 0)
    {
        qWarning() << "Unknown pose in distToPose";
        return 0.0;
    }

    return this->distToPoseIndex(indexOf, uavParams);
}

qreal Wayset::distToPoseIndex(int index, const UAVParameters &uavParams) const
{
    if (index <= 0 || index >= _poses.size())
    {
        qWarning() << "Bad index in distToPoseIndex";
        return 0.0;
    }

    const QList<Dubins> dubins = this->dubins(uavParams);

    qreal distSoFar = 0.0;
    for (int i = 0; i < index; i++)
    {
        const Dubins& dubin = dubins.at(i);
        distSoFar += dubin.length();
    }

    return distSoFar;
}

Wayset Wayset::portionByDist(qreal startDist, qreal endDist, const UAVParameters &uavParams) const
{
    Wayset toRet;
    const qreal maxDist = this->lengthMeters(uavParams);

    if (startDist < 0 || endDist < 0 || startDist >= endDist || endDist - maxDist > 0.001)
    {
        qWarning() << "Invalid arguments to Wayset::portionByDist" << startDist << endDist << maxDist;
        return toRet;
    }

    for (qreal current = startDist; current < endDist; current += uavParams.waypointInterval())
        toRet.append(this->sampleAtDistance(current,uavParams));

    toRet.append(this->sampleAtDistance(endDist, uavParams));

    return toRet;
}

Wayset Wayset::portionByTime(qreal startTime, qreal endTime, const UAVParameters &uavParams) const
{
    const qreal startDist = startTime * uavParams.airspeed();
    const qreal endDist = endTime * uavParams.airspeed();

    return this->portionByDist(startDist, endDist, uavParams);
}

Wayset Wayset::discretePortionByDist(qreal startDist, qreal endDist, const UAVParameters &uavParams) const
{
    Wayset toRet;
    const qreal maxDist = this->lengthMeters(uavParams);

    if (startDist < 0 || endDist < 0 || startDist >= endDist || endDist - maxDist > EPSILON_METERS)
    {
        qWarning() << "Invalid arguments to Wayset::portionByDist" << startDist << endDist << maxDist;
        return toRet;
    }

    const QList<Dubins> dubins = this->dubins(uavParams);
    qreal distSoFar = 0.0;
    for (int i = 0; i < dubins.size(); i++)
    {
        const Dubins& dubin = dubins.at(i);
        const qreal currentLength = dubin.length();

        const UAVPose& pose = _poses.at(i);

        if (distSoFar + currentLength > startDist
                && distSoFar + currentLength <= endDist)
            toRet.append(pose);
        else if (distSoFar >= startDist
                 && distSoFar <= endDist)
            toRet.append(pose);

        distSoFar += currentLength;
    }


    return toRet;
}

Wayset Wayset::discretePortionByTime(qreal startTime, qreal endTime, const UAVParameters &uavParams) const
{
    const qreal startDist = startTime * uavParams.airspeed();
    const qreal endDist = endTime * uavParams.airspeed();

    return this->discretePortionByDist(startDist, endDist, uavParams);
}

void Wayset::cleanup()
{
    QMutableListIterator<UAVPose> iter(_poses);

    bool gotPrev = false;
    UAVPose prev;

    int count = 0;
    while (iter.hasNext())
    {
        const UAVPose& current = iter.next();
        if (gotPrev && prev.pos().flatDistanceEstimate(current.pos()) < 5.0)
        {
            iter.remove();
            count++;
        }
        else
        {
            gotPrev = true;
            prev = current;
        }
    }
    qDebug() << "Wayset::cleanup removed" << count;
}

void Wayset::clear()
{
    _poses.clear();
}

void Wayset::append(const UAVPose &pose)
{
    if (this->isEmpty() || this->last() != pose)
        _poses.append(pose);
}

void Wayset::append(const Wayset &wayset)
{
    foreach(const UAVPose& pose, wayset.poses())
        this->append(pose);
}

void Wayset::append(const Position &pos,const UAVOrientation &angle)
{
    this->append(UAVPose(pos, angle));
}

void Wayset::prepend(const UAVPose &pose)
{
    if (this->isEmpty() || this->first() != pose)
        _poses.prepend(pose);
}

void Wayset::prepend(const Position &pos, const UAVOrientation &angle)
{
    this->prepend(UAVPose(pos, angle));
}

const QList<UAVPose> &Wayset::poses() const
{
    return _poses;
}

QList<Position> Wayset::positions() const
{
    QList<Position> toRet;

    foreach(const UAVPose& pose, this->poses())
        toRet.append(pose.pos());

    return toRet;
}

QList<UAVOrientation> Wayset::angles() const
{

    QList<UAVOrientation> toRet;

    foreach(const UAVPose& pose, this->poses())
        toRet.append(pose.angle());

    return toRet;
}

//pure-virtual from Serializable
QString Wayset::serializationType() const
{
    return "Wayset";
}

//pure-virtual from Serializable
void Wayset::serialize(QDataStream &stream) const
{
    const int numPoses = this->size();
    stream << numPoses;

    foreach(const UAVPose& pose, this->poses())
    {
        stream << pose.pos();
        pose.angle().serialize(stream);
    }
}
