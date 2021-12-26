#ifndef DUBINSINTERMEDIATEPLANNER_H
#define DUBINSINTERMEDIATEPLANNER_H

#include "HierarchicalPlanner/IntermediatePlanner.h"

class DubinsIntermediatePlanner : public IntermediatePlanner
{
public:
    DubinsIntermediatePlanner(const UAVParameters& uavParams,
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

#endif // DUBINSINTERMEDIATEPLANNER_H
