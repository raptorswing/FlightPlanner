#include "RRTIntermediatePlanner.h"

#include <limits>
#include <cmath>

#include "guts/Conversions.h"

const qreal EVERY_X_METERS = 50.0;
const qreal AIRSPEED = 14.0; //meters per second
const qreal TIMESLICE = 15.0; //seconds
const qreal MAX_TURN_ANGLE = 3.14159265 / 4.0;

RRTIntermediatePlanner::RRTIntermediatePlanner(const Position &startPos,
                                               const UAVOrientation &startPose,
                                               const Position &endPos,
                                               const UAVOrientation &endPose,
                                               const QList<QPolygonF> &obstacles) :
    IntermediatePlanner(startPos, startPose, endPos, endPose, obstacles)
{
}

bool RRTIntermediatePlanner::plan()
{
    _results.clear();

    const QVector3D goal = _toVec(this->endPos(), this->endPose());

    QList<QVector3D> nodes;
    QHash<QVector3D, QVector3D> parents;
    nodes.append(_toVec(this->startPos(), this->startPose()));

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
        QVector3D nearestExisting;
        qreal nearestExistingDist = std::numeric_limits<qreal>::max();
        foreach(const QVector3D& existing, nodes)
        {
            qreal dist = _distance(existing, random);
            if (dist < nearestExistingDist)
            {
                nearestExistingDist = dist;
                nearestExisting = existing;
            }
        }

        //Generate step towards random from nearestExisting
        const Position existingPos = _toPosition(nearestExisting);
        const UAVOrientation existingPose = _toOrientation(nearestExisting);

        QVector3D bestNew;
        qreal bestNewDist = std::numeric_limits<qreal>::max();

        const int branches = 1;
        for (int i = -branches; i <= branches; i++)
        {
            const qreal angleMod = MAX_TURN_ANGLE * ((qreal)i / (qreal)branches);
            const qreal successorRadians = existingPose.radians() + angleMod;
            QVector3D translateVec(cos(successorRadians), sin(successorRadians), 0);
            translateVec.normalize();
            translateVec *= EVERY_X_METERS;
            const Position successorPos(existingPos.longitude() + lonPerMeter * translateVec.x(),
                                        existingPos.latitude() + latPerMeter * translateVec.y());
            const UAVOrientation successorPose(successorRadians);
            const QVector3D vec = _toVec(successorPos, successorPose);

            //No flying through obstacles!
            if (_collidesWithObstacle(successorPos))
                continue;

            const qreal dist = _distance(random, vec);
            if (dist < bestNewDist)
            {
                bestNewDist = dist;
                bestNew = vec;
            }
        }

        if (bestNew.isNull())
            continue;
        nodes.append(bestNew);
        parents.insert(bestNew, nearestExisting);

        const qreal distToGoal = _distance(goal, bestNew);
        if (distToGoal < 50.0)
        {
            qDebug() << "Solution found - trace back";
            QVector3D current = bestNew;
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
QVector3D RRTIntermediatePlanner::_toVec(const Position &pos,
                                         const UAVOrientation &pose)
{
    return QVector3D(pos.longitude(), pos.latitude(), pose.radians());
}

//private static
qreal RRTIntermediatePlanner::_distance(const QVector3D &a, const QVector3D &b)
{
    const qreal angleFactor = 500.0;
    const qreal lonPerMeter = Conversions::degreesLonPerMeter(a.y());
    const qreal latPerMeter = Conversions::degreesLatPerMeter(a.y());
    qreal toRet = sqrt(pow((a.x() - b.x()) / lonPerMeter, 2.0)
                       + pow((a.y() - b.y()) / latPerMeter, 2.0)
                       + angleFactor * pow(a.z() - b.z(), 2.0));

    return toRet;
}

//private static
UAVOrientation RRTIntermediatePlanner::_toOrientation(const QVector3D &vec)
{
    return UAVOrientation(vec.z());
}

//private static
Position RRTIntermediatePlanner::_toPosition(const QVector3D &vec)
{
    return Position(vec.toPointF());
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
