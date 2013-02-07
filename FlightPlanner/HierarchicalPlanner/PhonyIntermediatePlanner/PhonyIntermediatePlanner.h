#ifndef PHONYINTERMEDIATEPLANNER_H
#define PHONYINTERMEDIATEPLANNER_H

#include "HierarchicalPlanner/IntermediatePlanner.h"
#include "UAVParameters.h"

class PhonyIntermediatePlanner : public IntermediatePlanner
{
public:
    PhonyIntermediatePlanner(const UAVParameters &uavParams,
                             const Position& startPos,
                             const UAVOrientation& startPose,
                             const Position& endPos,
                             const UAVOrientation& endPose,
                             const QList<QPolygonF>& obstacles);

    virtual bool plan();
    virtual QList<Position> results() const;

private:
    UAVParameters _uavParams;
    QList<Position> _results;
};

#endif // PHONYINTERMEDIATEPLANNER_H
