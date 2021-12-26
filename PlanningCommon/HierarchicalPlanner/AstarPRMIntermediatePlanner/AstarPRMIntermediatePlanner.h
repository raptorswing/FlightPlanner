#ifndef ASTARPRMINTERMEDIATEPLANNER_H
#define ASTARPRMINTERMEDIATEPLANNER_H

#include "HierarchicalPlanner/IntermediatePlanner.h"
#include "UAVParameters.h"

class AstarPRMIntermediatePlanner : public IntermediatePlanner
{
public:
    AstarPRMIntermediatePlanner(const UAVParameters& uavParams,
                                const Position& startPos,
                                const UAVOrientation& startAngle,
                                const Position& endPos,
                                const UAVOrientation& endAngle,
                                const QList<QPolygonF>& obstacles);

    void setAstarGranularity(qreal granularity);
    qreal granularity() const;

    //pure-virtual from IntermediatePlanner
    virtual bool plan();

    //pure-virtual from IntermediatePlanner
    virtual Wayset results() const;

private:
    bool _collidesWithObstacle(const Position& candidate, const Position& parent);
    void _toRealPath(const Wayset &metaPlan);

    Wayset _results;

    qreal _granularity;

};

#endif // ASTARPRMINTERMEDIATEPLANNER_H
