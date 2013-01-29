#ifndef RRTINTERMEDIATEPLANNER_H
#define RRTINTERMEDIATEPLANNER_H

#include "HierarchicalPlanner/IntermediatePlanner.h"

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
    static QVector3D _toVec(const Position& pos, const UAVOrientation& pose);
    static qreal _distance(const QVector3D& a, const QVector3D& b);
    static UAVOrientation _toOrientation(const QVector3D& vec);
    static Position _toPosition(const QVector3D& vec);

    bool _collidesWithObstacle(const Position& pos) const;

    QList<Position> _results;
};

#endif // RRTINTERMEDIATEPLANNER_H
