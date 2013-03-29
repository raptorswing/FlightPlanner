#include "AstarPRMIntermediatePlanner.h"

const qreal GRANULARITY = 300.0;

#include <QMultiMap>
#include <QSet>
#include <cmath>

#include "QVectorND.h"
#include "guts/Conversions.h"
#include "HierarchicalPlanner/DubinsIntermediate/DubinsIntermediatePlanner.h"
#include "Dubins.h"

AstarPRMIntermediatePlanner::AstarPRMIntermediatePlanner(const UAVParameters& uavParams,
                                                         const Position &startPos,
                                                         const UAVOrientation &startPose,
                                                         const Position &endPos,
                                                         const UAVOrientation &endPose,
                                                         const QList<QPolygonF> &obstacles) :
    IntermediatePlanner(uavParams, startPos, startPose, endPos, endPose, obstacles)
{
}

bool AstarPRMIntermediatePlanner::plan()
{
    _results.clear();

    const qreal lonPerMeter = Conversions::degreesLonPerMeter(this->startPos().latitude());
    const qreal latPerMeter = Conversions::degreesLatPerMeter(this->startPos().latitude());

    QMultiMap<qreal, Position> workList;
    QHash<Position, Position> parents;
    QSet<Position> closedSet;
    QHash<Position, qreal> costSoFar;

    workList.insert(0, this->startPos());
    costSoFar.insert(this->startPos(), 0);

    while (!workList.empty())
    {
        const qreal bestScore = workList.keys().first();
        const Position current = workList.value(bestScore);
        workList.remove(bestScore, current);
        closedSet.insert(current);

        qDebug() << "A* intermed:" << current << bestScore;

        //When we get close enough trace back
        if (current.flatDistanceEstimate(this->endPos()) < GRANULARITY)
        {
            Wayset metaPlan;
            metaPlan.append(this->endPos());

            //Trace back
            Position trace = current;
            while (true)
            {
                metaPlan.prepend(trace);
                if (!parents.contains(trace))
                    break;
                trace = parents.value(trace);
            }
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
                const Position neighbor(current.longitude() + xd * GRANULARITY * lonPerMeter,
                                        current.latitude() + xy * GRANULARITY * latPerMeter);

                //Check closed set
                if (closedSet.contains(neighbor))
                    continue;

                //Check obstacles
                bool obstacleViolation = false;
                foreach(const QPolygonF& obstacle, this->obstacles())
                {
                    if (obstacle.containsPoint(neighbor.lonLat(), Qt::OddEvenFill))
                    {
                        obstacleViolation = true;
                        break;
                    }
                }
                if (obstacleViolation)
                    break;

                const qreal tentativeCost = costSoFar.value(current) + GRANULARITY;
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

Wayset AstarPRMIntermediatePlanner::results() const
{
    return _results;
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

    qreal totalDubinLength = 0.0;
    QList<Dubins> dubins;
    for (int i = 0; i < metaPlan.size() - 1; i++)
    {
        const Position& startPos = metaPlan.at(i).pos();
        const Position& endPos = metaPlan.at(i + 1).pos();
        const QPointF startMeters(0.0, 0.0);
        const QPointF endMeters = startPos.flatOffsetMeters(endPos).toPointF();

        const UAVOrientation& startAngle = orientations.at(i);
        const UAVOrientation& endAngle = orientations.at(i + 1);
        Dubins d(startMeters, startAngle.radians(), endMeters, endAngle.radians(),
                 this->uavParams().minTurningRadius());
        dubins.append(d);
        totalDubinLength += d.length();
    }

    const int numSamples = qRound(totalDubinLength / this->uavParams().waypointInterval());
    qDebug() << "Do" << numSamples << "samples";
    for (int i = 0; i < numSamples; i++)
    {
        const qreal totalSamplePos = this->uavParams().waypointInterval() * i;

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
                dubinRoot = metaPlan.at(j).pos();
                break;
            }
            groundCovered += d.length();
        }

        if (!current.isValid())
            qWarning() << this << "has invalid dubins!";

        QPointF sampleOffset;
        qreal angleResult;
        current.sample(totalSamplePos - groundCovered, sampleOffset, angleResult);

        Position sampleResult = dubinRoot.flatOffsetToPosition(sampleOffset);
        _results.append(Position(sampleResult), UAVOrientation(angleResult));
    }
}
