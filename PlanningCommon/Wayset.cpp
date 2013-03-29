#include "Wayset.h"

#include <QtDebug>
#include <QVector2D>

Wayset::Wayset()
{
}

Wayset::Wayset(const QList<UAVPose> &waypoints) :
    _poses(waypoints)
{
}

const UAVPose &Wayset::at(int i) const
{
    if (i < 0 || i >= this->size())
        qWarning() << "Index" << i << "out of bounds!";
    return _poses.at(i);
}

const UAVPose &Wayset::first() const
{
    return _poses.last();
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

    const QList<Dubins> dubins = this->dubins(uavParams);
    const qreal totalLengthMeters = this->lengthMeters(uavParams);
    const int numSamples = qRound(totalLengthMeters / granularityMeters);

    for (int i = 0; i < numSamples; i++)
    {
        const qreal totalSamplePos = granularityMeters * i;

        qreal groundCovered = 0.0;
        Dubins current;
        Position dubinRoot;
        for (int j = 0; j < dubins.size(); j++)
        {
            const Dubins & d = dubins.at(j);
            const qreal extent = groundCovered + d.length();
            if (extent > totalSamplePos)
            {
                current = d;
                dubinRoot = this->at(j).pos();
                break;
            }
            groundCovered += d.length();
        }
        if (!current.isValid())
            qWarning() << "Wayset has invalid dubins!";

        QPointF sampleOffset;
        qreal angleResult;
        current.sample(totalSamplePos - groundCovered, sampleOffset, angleResult);

        const Position sampleResult = dubinRoot.flatOffsetToPosition(sampleOffset);
        const UAVOrientation sampleResultAngle(angleResult);
        toRet.append(UAVPose(sampleResult, sampleResultAngle));
    }

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

void Wayset::clear()
{
    _poses.clear();
}

void Wayset::append(const UAVPose &pos)
{
    _poses.append(pos);
}

void Wayset::append(const Wayset &wayset)
{
    _poses.append(wayset.poses());
}

void Wayset::append(const Position &pos,const UAVOrientation &angle)
{
    _poses.append(UAVPose(pos, angle));
}

void Wayset::prepend(const UAVPose &pos)
{
    _poses.prepend(pos);
}

void Wayset::prepend(const Position &pos, const UAVOrientation &angle)
{
    _poses.prepend(UAVPose(pos, angle));
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
