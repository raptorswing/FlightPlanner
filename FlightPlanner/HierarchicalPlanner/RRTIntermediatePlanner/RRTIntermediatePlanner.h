#ifndef RRTINTERMEDIATEPLANNER_H
#define RRTINTERMEDIATEPLANNER_H

#include "HierarchicalPlanner/IntermediatePlanner.h"

#include "QVectorND.h"

class RRTIntermediatePlanner : public IntermediatePlanner
{
public:
    RRTIntermediatePlanner(const Position& startPos,
                           const UAVOrientation& startPose,
                           const Position& endPos,
                           const UAVOrientation& endPose,
                           const QList<QPolygonF>& obstacles);

    virtual bool plan();
    virtual QList<Position> results() const;

private:
    static QVectorND _toVec(const Position& pos, const UAVOrientation& pose);
    static UAVOrientation _toOrientation(const QVectorND &vec);
    static Position _toPosition(const QVectorND& vec);

    bool _collidesWithObstacle(const Position& pos) const;

    QList<Position> _results;
};

#endif // RRTINTERMEDIATEPLANNER_H
