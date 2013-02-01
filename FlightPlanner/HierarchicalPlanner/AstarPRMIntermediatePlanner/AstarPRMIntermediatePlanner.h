#ifndef ASTARPRMINTERMEDIATEPLANNER_H
#define ASTARPRMINTERMEDIATEPLANNER_H

#include "HierarchicalPlanner/IntermediatePlanner.h"

class AstarPRMIntermediatePlanner : public IntermediatePlanner
{
public:
    AstarPRMIntermediatePlanner(const Position& startPos,
                                const UAVOrientation& startPose,
                                const Position& endPos,
                                const UAVOrientation& endPose,
                                const QList<QPolygonF>& obstacles);

    virtual bool plan();
    virtual QList<Position> results() const;

private:
    QList<Position> _results;

};

#endif // ASTARPRMINTERMEDIATEPLANNER_H
