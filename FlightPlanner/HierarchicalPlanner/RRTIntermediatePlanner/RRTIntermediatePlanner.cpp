#include "RRTIntermediatePlanner.h"

#include <limits>
#include <cmath>
#include <QVector2D>

#include "guts/Conversions.h"
#include "QKDTree.h"
#include "RRTDistanceMetric.h"

RRTIntermediatePlanner::RRTIntermediatePlanner(const UAVParameters& uavParams,
                                               const Position &startPos,
                                               const UAVOrientation &startPose,
                                               const Position &endPos,
                                               const UAVOrientation &endPose,
                                               const QList<QPolygonF> &obstacles) :
    IntermediatePlanner(uavParams, startPos, startPose, endPos, endPose, obstacles)
{
}

bool RRTIntermediatePlanner::plan()
{
    _results.clear();

    const QVectorND goal = _toVec(this->endPos(), this->endPose());

    QKDTree kdtree(3, false, new RRTDistanceMetric(this->endPos().latitude()));

    QHash<QVectorND, QVectorND> parents;
    kdtree.add(_toVec(this->startPos(), this->startPose()), 1);

    const quint32 squareSize = 5000;
    const qreal lonPerMeter = Conversions::degreesLonPerMeter(this->startPos().latitude());
    const qreal latPerMeter = Conversions::degreesLatPerMeter(this->startPos().latitude());

    int count = 0;
    while (count++ < 50000)
    {
        //Generate random place in state space
        const int lonDiff = (qrand() % squareSize) + 1 - squareSize / 2;
        const int latDiff = (qrand() % squareSize) + 1 - squareSize / 2;
        const QVector3D random(this->startPos().longitude() + lonDiff * lonPerMeter,
                               this->startPos().latitude() + latDiff * latPerMeter,
                               (qreal)qrand()/((qreal)RAND_MAX/(2.0*3.14159265)));

        //Find nearest existing node
        QVectorND nearestExisting;
        kdtree.nearestKey(random, &nearestExisting);
        if (nearestExisting == random)
            continue;

        //Generate step towards random from nearestExisting
        const Position existingPos = _toPosition(nearestExisting);
        const UAVOrientation existingPose = _toOrientation(nearestExisting);

        QVectorND bestNew(kdtree.dimension());
        qreal bestNewDist = std::numeric_limits<qreal>::max();

        const int branches = 1;
        for (int i = -branches; i <= branches; i++)
        {
            const qreal angleMod = this->uavParams().maxTurnAngle() * ((qreal)i / (qreal)branches);
            const qreal successorRadians = existingPose.radians() + angleMod;
            QVector2D translateVec(cos(successorRadians), sin(successorRadians));
            translateVec.normalize();
            translateVec *= this->uavParams().waypointInterval();
            const Position successorPos(existingPos.longitude() + lonPerMeter * translateVec.x(),
                                        existingPos.latitude() + latPerMeter * translateVec.y());
            const UAVOrientation successorPose(successorRadians);
            const QVectorND vec = _toVec(successorPos, successorPose);

            //No flying through obstacles!
            if (_collidesWithObstacle(successorPos))
                continue;

            const qreal dist = kdtree.distanceMetric()->distance(random, vec);
            if (dist < bestNewDist)
            {
                bestNewDist = dist;
                bestNew = vec;
            }
        }

        if (bestNew.isNull())
            continue;
        kdtree.add(bestNew,2);
        parents.insert(bestNew, nearestExisting);

        const qreal distToGoal = kdtree.distanceMetric()->distance(goal, bestNew);
        if (distToGoal < 50.0)
        {
            qDebug() << "Solution found - trace back";
            QVectorND current = bestNew;
            while (true)
            {
                _results.prepend(_toPosition(current));
                if (parents.contains(current))
                    current = parents.value(current);
                else
                    break;
            }
            break;
        }
    }

    if (_results.isEmpty())
        return false;
    return true;
}

QList<Position> RRTIntermediatePlanner::results() const
{
    return _results;
}

//private static
QVectorND RRTIntermediatePlanner::_toVec(const Position &pos,
                                         const UAVOrientation &pose)
{
    return QVectorND(QVector3D(pos.longitude(), pos.latitude(), pose.radians()));
}

//private static
UAVOrientation RRTIntermediatePlanner::_toOrientation(const QVectorND &vec)
{
    return UAVOrientation(vec.val(2));
}

//private static
Position RRTIntermediatePlanner::_toPosition(const QVectorND &vec)
{
    Position toRet;
    toRet.setLongitude(vec.val(0));
    toRet.setLatitude(vec.val(1));
    return toRet;
}

//private
bool RRTIntermediatePlanner::_collidesWithObstacle(const Position &pos) const
{
    foreach(const QPolygonF& obs, this->obstacles())
    {
        if (obs.containsPoint(pos.lonLat(), Qt::OddEvenFill))
            return true;
    }
    return false;
}

uint qHash(const QVector3D& vec)
{
    return vec.x() + vec.y() + vec.z();
}
