#include "PhonyIntermediatePlanner.h"

#include <QVector2D>
#include <QtCore>

#include "guts/Conversions.h"

PhonyIntermediatePlanner::PhonyIntermediatePlanner(const UAVParameters& uavParams,
                                                   const Position &startPos,
                                                   const UAVOrientation &startPose,
                                                   const Position &endPos,
                                                   const UAVOrientation &endPose,
                                                   const QList<QPolygonF> &obstacles) :
    IntermediatePlanner(uavParams, startPos, startPose, endPos, endPose, obstacles)
{
}

bool PhonyIntermediatePlanner::plan()
{
    _results.clear();

    const qreal lonPerMeter = Conversions::degreesLonPerMeter(this->startPos().latitude());
    const qreal latPerMeter = Conversions::degreesLatPerMeter(this->startPos().latitude());

    QVector2D dirVec((this->endPos().longitude() - this->startPos().longitude()) / lonPerMeter,
                     (this->endPos().latitude() - this->startPos().latitude()) / latPerMeter);
    const qreal distMeters = dirVec.length();
    dirVec.normalize();

    const int intervals = qCeil(distMeters / this->uavParams().waypointInterval());
    for (int i = 0; i < intervals; i++)
    {
        const QVector2D moveVec = dirVec * this->uavParams().waypointInterval() * i;
        const Position newPos(this->startPos().longitude() + moveVec.x() * lonPerMeter,
                              this->startPos().latitude() + moveVec.y() * latPerMeter);
        _results.append(newPos);
    }

    return true;
}

Wayset PhonyIntermediatePlanner::results() const
{
    return _results;
}
