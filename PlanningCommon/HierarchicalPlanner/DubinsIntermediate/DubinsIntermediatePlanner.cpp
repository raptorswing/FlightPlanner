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

    const QPointF startPos(0.0001, 0.0001);
    const qreal startAngle = this->startAngle().radians();
    const QPointF endPos = this->startPos().flatOffsetMeters(this->endPos()).toPointF();
    const qreal endAngle = this->endAngle().radians();
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

        const Position pos = this->startPos().flatOffsetToPosition(samplePos);
        const UAVPose pose(pos, sampleAngle);
        _results.append(pose);
    }

    return true;
}

//virtual from IntermediatePlanner
Wayset DubinsIntermediatePlanner::results() const
{
    return _results;
}
