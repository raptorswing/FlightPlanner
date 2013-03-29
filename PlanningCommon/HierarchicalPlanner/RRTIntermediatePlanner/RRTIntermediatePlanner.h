#ifndef RRTINTERMEDIATEPLANNER_H
#define RRTINTERMEDIATEPLANNER_H

#include "HierarchicalPlanner/IntermediatePlanner.h"
#include "UAVParameters.h"
#include "QVectorND.h"

class RRTIntermediatePlanner : public IntermediatePlanner
{
public:
    RRTIntermediatePlanner(const UAVParameters& uavParams,
                           const Position& startPos,
                           const UAVOrientation& startAngle,
                           const Position& endPos,
                           const UAVOrientation& endAngle,
                           const QList<QPolygonF>& obstacles);

    virtual bool plan();
    virtual Wayset results() const;

private:
    static QVectorND _toVec(const Position& pos, const UAVOrientation& pose);
    static UAVOrientation _toOrientation(const QVectorND &vec);
    static Position _toPosition(const QVectorND& vec);

    bool _collidesWithObstacle(const Position& pos) const;

    Wayset _results;
};

#endif // RRTINTERMEDIATEPLANNER_H
