#ifndef ASTARPRMINTERMEDIATEPLANNER_H
#define ASTARPRMINTERMEDIATEPLANNER_H

#include "HierarchicalPlanner/IntermediatePlanner.h"
#include "UAVParameters.h"

class AstarPRMIntermediatePlanner : public IntermediatePlanner
{
public:
    AstarPRMIntermediatePlanner(const UAVParameters& uavParams,
                                const Position& startPos,
                                const UAVOrientation& startPose,
                                const Position& endPos,
                                const UAVOrientation& endPose,
                                const QList<QPolygonF>& obstacles);

    virtual bool plan();
    virtual Wayset results() const;

private:
    void _toRealPath(const Wayset &metaPlan);

    Wayset _results;

};

#endif // ASTARPRMINTERMEDIATEPLANNER_H
