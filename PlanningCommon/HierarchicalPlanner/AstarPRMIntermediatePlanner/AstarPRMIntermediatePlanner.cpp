#include "AstarPRMIntermediatePlanner.h"

const qreal DEFAULT_GRANULARITY = 150.0;

#include <QMultiMap>
#include <QSet>
#include <QtCore>
#include <cmath>

#include "QVectorND.h"
#include "guts/Conversions.h"
#include "HierarchicalPlanner/SmartIntermediatePlanner.h"
#include "Dubins.h"

AstarPRMIntermediatePlanner::AstarPRMIntermediatePlanner(const UAVParameters& uavParams,
                                                         const Position &startPos,
                                                         const UAVOrientation &startPose,
                                                         const Position &endPos,
                                                         const UAVOrientation &endPose,
                                                         const QList<QPolygonF> &obstacles) :
    IntermediatePlanner(uavParams, startPos, startPose, endPos, endPose, obstacles)
{
    this->setAstarGranularity(DEFAULT_GRANULARITY);
}

void AstarPRMIntermediatePlanner::setAstarGranularity(qreal granularity)
{
    _granularity = qMax<qreal>(25.0, granularity);
}

qreal AstarPRMIntermediatePlanner::granularity() const
{
    return _granularity;
}

//pure-virtual from IntermediatePlanner
bool AstarPRMIntermediatePlanner::plan()
{
    _results.clear();

    QMultiMap<qreal, Position> workList;
    QHash<Position, Position> parents;
    QHash<Position, UAVOrientation> angles;
    QSet<Position> closedSet;
    QHash<Position, qreal> costSoFar;

    workList.insert(0, this->startPos());
    costSoFar.insert(this->startPos(), 0);
    angles.insert(this->startPos(), this->startAngle());

    while (!workList.empty())
    {
        const qreal bestScore = workList.keys().first();
        const Position current = workList.value(bestScore);
        workList.remove(bestScore, current);
        closedSet.insert(current);

        //qDebug() << "A* intermed:" << current << bestScore;

        //When we get close enough trace back
        if (current.flatDistanceEstimate(this->endPos()) < this->granularity())
        {
            Wayset metaPlan;

            //Trace back
            Position trace = current;
            while (true)
            {
                metaPlan.prepend(trace);
                if (!parents.contains(trace))
                    break;
                trace = parents.value(trace);
            }
            qDebug() << "A* intermed length" << metaPlan.size();
            _toRealPath(metaPlan);
            return true;
        }

        //Otherwise generate more neighbors!
        for (int xd = -1; xd <= 1; xd++)
        {
            for (int xy = -1; xy <= 1; xy++)
            {
                if (xd == 0 && xy == 0)
                    continue;
                const Position neighbor = current.flatOffsetToPosition(QPointF(xd * this->granularity(),
                                                                               xy * this->granularity()));

                //Check closed set
                if (closedSet.contains(neighbor))
                    continue;

                //Check obstacles
                if (_collidesWithObstacle(neighbor, current))
                    continue;

                qreal costToMove = this->granularity();
                if (xd != 0 && xy != 0)
                    costToMove = sqrt(2.0 * this->granularity() * this->granularity());

                const qreal tentativeCost = costSoFar.value(current) + costToMove;
                if (!costSoFar.contains(neighbor) || tentativeCost < costSoFar.value(neighbor))
                {
                    parents.insert(neighbor, current);
                    costSoFar.insert(neighbor, tentativeCost);
                    workList.insert(tentativeCost + neighbor.flatDistanceEstimate(this->endPos()),
                                    neighbor);
                }
            }
        }
    }

    return false;
}

//pure-virtual from IntermediatePlanner
Wayset AstarPRMIntermediatePlanner::results() const
{
    return _results;
}

bool AstarPRMIntermediatePlanner::_collidesWithObstacle(const Position &candidate, const Position &parent)
{
    const qreal checkGranularity = 10.0;

    foreach(const QPolygonF& obsPoly, this->obstacles())
    {
        if (obsPoly.containsPoint(candidate.lonLat(), Qt::OddEvenFill))
            return true;
        else if (obsPoly.containsPoint(parent.lonLat(), Qt::OddEvenFill))
            return true;
        else if (candidate == parent)
            return false;

        const UAVOrientation& angle = parent.angleTo(candidate);
        const qreal totalDist = parent.flatDistanceEstimate(candidate);
        const int numSteps = qCeil(totalDist / checkGranularity);

        for (int i = 0; i < numSteps; i++)
        {
            const QPointF offset(cos(angle.radians()) * i * checkGranularity,
                                 sin(angle.radians()) * i * checkGranularity);
            const Position& checkPos = parent.flatOffsetToPosition(offset);

            if (obsPoly.containsPoint(checkPos.lonLat(), Qt::OddEvenFill))
                return true;
        }
    }
    return false;
}

//private
void AstarPRMIntermediatePlanner::_toRealPath(const Wayset& metaPlan)
{
    QList<UAVOrientation> orientations;
    orientations.append(this->startAngle());
    for (int i = 1; i < metaPlan.size() - 1; i++)
    {
        const Position& prev = metaPlan.at(i-1).pos();
        const Position& current = metaPlan.at(i).pos();
        const Position& next = metaPlan.at(i+1).pos();

        const UAVOrientation prevAngle(prev.angleTo(current));
        const UAVOrientation nextAngle(current.angleTo(next));
        const UAVOrientation avg = UAVOrientation::average(prevAngle, nextAngle);

        orientations.append(UAVOrientation(avg));
    }
    orientations.append(this->endAngle());

    Wayset temp;
    for (int i = 0; i < metaPlan.size(); i++)
    {
        temp.append(metaPlan.at(i).pos(),
                    orientations.at(i));
    }
    temp.append(this->endPos(), this->endAngle());

    UAVParameters fudgeParams = this->uavParams();
    fudgeParams.setMinTurningRadius(fudgeParams.minTurningRadius() * 1.02);
    _results = temp.resample(fudgeParams.waypointInterval(), fudgeParams);
}
