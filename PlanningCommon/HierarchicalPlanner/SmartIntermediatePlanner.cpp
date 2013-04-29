#include "SmartIntermediatePlanner.h"

#include <QScopedPointer>

#include "AstarPRMIntermediatePlanner/AstarPRMIntermediatePlanner.h"
#include "DubinsIntermediate/DubinsIntermediatePlanner.h"

SmartIntermediatePlanner::SmartIntermediatePlanner(const UAVParameters &uavParams,
                                                   const Position &startPos,
                                                   const UAVOrientation &startAngle,
                                                   const Position &endPos,
                                                   const UAVOrientation &endAngle,
                                                   const QList<QPolygonF> &obstacles) :
    IntermediatePlanner(uavParams, startPos, startAngle, endPos, endAngle, obstacles),
    _customGranularity(false)
{
}

void SmartIntermediatePlanner::setAstarGranularity(qreal granularity)
{
    _customGranularity = true;
    _granularity = qMax<qreal>(25.0, granularity);
}

//pure-virtual from IntermediatePlanner
bool SmartIntermediatePlanner::plan()
{
    _results.clear();

    QScopedPointer<DubinsIntermediatePlanner> dubinPlanner(new DubinsIntermediatePlanner(this->uavParams(),
                                                                                         this->startPos(),
                                                                                         this->startAngle(),
                                                                                         this->endPos(),
                                                                                         this->endAngle(),
                                                                                         this->obstacles()));

    /*
     *if the dubin planner fails we give up as we have no reason to believe the A* planner
     *will work any better...
    */
    if (!dubinPlanner->plan())
        return false;

    //Check for obstacle violations
    Wayset dubinResultsResampled = dubinPlanner->results().resample(this->uavParams().waypointInterval(),
                                                                    this->uavParams());

    bool obstacleViolated = false;
    const QList<Position> positions = dubinResultsResampled.positions();
    foreach(const Position& pos, positions)
    {
        const QPointF lonLat = pos.lonLat();
        foreach(const QPolygonF& obsPoly, this->obstacles())
        {
            if (obsPoly.containsPoint(lonLat, Qt::OddEvenFill))
            {
                obstacleViolated = true;
                break;
            }
        }

        if (obstacleViolated)
            break;
    }

    //If the dubins planner violated obstacles then we can return. Otherwise we need to use the A* planner
    if (!obstacleViolated)
    {
        _results = dubinPlanner->results();
        return true;
    }

    QScopedPointer<AstarPRMIntermediatePlanner> aStar(new AstarPRMIntermediatePlanner(this->uavParams(),
                                                                                      this->startPos(),
                                                                                      this->startAngle(),
                                                                                      this->endPos(),
                                                                                      this->endAngle(),
                                                                                      this->obstacles()));
    if (_customGranularity)
        aStar->setAstarGranularity(_granularity);

    if (!aStar->plan())
        return false;
    _results = aStar->results();

    return true;
}

//pure-virtual from IntermediatePlanner
Wayset SmartIntermediatePlanner::results() const
{
    return _results;
}
