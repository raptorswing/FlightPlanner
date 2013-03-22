#ifndef DUBINSINTERMEDIATEPLANNER_H
#define DUBINSINTERMEDIATEPLANNER_H

#include "HierarchicalPlanner/IntermediatePlanner.h"

class DubinsIntermediatePlanner : public IntermediatePlanner
{
public:
    DubinsIntermediatePlanner(const UAVParameters& uavParams,
                              const Position& startPos,
                              const UAVOrientation& startPose,
                              const Position& endPos,
                              const UAVOrientation& endPose,
                              const QList<QPolygonF>& obstacles);

    virtual bool plan();
    virtual QList<Position> results() const;

private:
    QList<Position> _results;
};

#endif // DUBINSINTERMEDIATEPLANNER_H
