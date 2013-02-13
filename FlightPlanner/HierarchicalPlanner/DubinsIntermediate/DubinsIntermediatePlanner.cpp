#include "DubinsIntermediatePlanner.h"

#include "dubins.h"
#include "guts/Conversions.h"

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

    DubinsPath path;

    //Some added noise to start so that start pos != end pos, which breaks the dubin solver
    qreal start[3] = {0.001, 0.001, this->startPose().radians()};
    qreal end[3] = {(this->endPos().longitude() - this->startPos().longitude()) / lonPerMeter,
                    (this->endPos().latitude() - this->startPos().latitude()) / latPerMeter,
                   this->endPose().radians()};
    const qreal rho = this->uavParams().minTurningRadius();

    //Build the path
    if (dubins_init(start, end, rho, &path) != 0)
        return false;

    const qreal lengthMeters = dubins_path_length(&path);
    const int numSamples = qCeil(lengthMeters / this->uavParams().waypointInterval());

    //Convert back to lat/lon
    for (int i = 0; i < numSamples; i++)
    {
        const qreal t = i * this->uavParams().waypointInterval();
        qreal result[3];
        if (dubins_path_sample(&path, t, result) != 0)
            return false;

        Position pos(this->startPos().longitude() + result[0] * lonPerMeter,
                this->startPos().latitude() + result[1] * latPerMeter);
        _results.append(pos);
    }

    return true;
}

//virtual from IntermediatePlanner
QList<Position> DubinsIntermediatePlanner::results() const
{
    return _results;
}
