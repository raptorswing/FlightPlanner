#include "DubinsIntermediatePlanner.h"

#include "guts/Conversions.h"
#include "Dubins.h"
#include <QtCore>

DubinsIntermediatePlanner::DubinsIntermediatePlanner(const UAVParameters &uavParams,
                                                     const Position &startPos,
                                                     const UAVOrientation &startPose,
                                                     const Position &endPos,
                                                     const UAVOrientation &endPose,
                                                     const QList<QPolygonF> &obstacles)
    : IntermediatePlanner(uavParams, startPos, startPose, endPos, endPose, obstacles)
{
}

//virtual from IntermediatePlanner
bool DubinsIntermediatePlanner::plan()
{
    _results.clear();

    const qreal lonPerMeter = Conversions::degreesLonPerMeter(this->startPos().latitude());
    const qreal latPerMeter = Conversions::degreesLatPerMeter(this->startPos().latitude());

    const QPointF startPos(0.0001, 0.0001);
    const qreal startAngle = this->startPose().radians();
    const QPointF endPos((this->endPos().longitude() - this->startPos().longitude()) / lonPerMeter,
                         (this->endPos().latitude() - this->startPos().latitude()) / latPerMeter);
    const qreal endAngle = this->endPose().radians();
    const qreal minTurnRadius = this->uavParams().minTurningRadius();

    Dubins dubins(startPos, startAngle, endPos, endAngle, minTurnRadius);

    //Build the path
    if (!dubins.isValid())
        return false;

    const qreal lengthMeters = dubins.length();
    const int numSamples = qRound(lengthMeters / this->uavParams().waypointInterval());

    //Convert back to lat/lon
    for (int i = 0; i < numSamples; i++)
    {
        const qreal t = i * this->uavParams().waypointInterval();
        QPointF samplePos;
        qreal sampleAngle;
        if (!dubins.sample(t, samplePos, sampleAngle))
            return false;

        Position pos(this->startPos().longitude() + samplePos.x() * lonPerMeter,
                this->startPos().latitude() + samplePos.y() * latPerMeter);
        _results.append(pos);
    }

    return true;
}

//virtual from IntermediatePlanner
QList<Position> DubinsIntermediatePlanner::results() const
{
    return _results;
}
