#ifndef PHONYINTERMEDIATEPLANNER_H
#define PHONYINTERMEDIATEPLANNER_H

#include "HierarchicalPlanner/IntermediatePlanner.h"
#include "UAVParameters.h"

class PhonyIntermediatePlanner : public IntermediatePlanner
{
public:
    PhonyIntermediatePlanner(const UAVParameters &uavParams,
                             const Position& startPos,
                             const UAVOrientation& startAngle,
                             const Position& endPos,
                             const UAVOrientation& endAngle,
                             const QList<QPolygonF>& obstacles);

    virtual bool plan();
    virtual Wayset results() const;

private:
    Wayset _results;
};

#endif // PHONYINTERMEDIATEPLANNER_H
