#ifndef INTERMEDIATEPLANNER_H
#define INTERMEDIATEPLANNER_H

#include "Position.h"
#include "UAVOrientation.h"
#include "UAVParameters.h"

#include <QList>
#include <QPolygonF>

class IntermediatePlanner
{
public:
    IntermediatePlanner(const UAVParameters& uavParams,
                        const Position& startPos,
                        const UAVOrientation& startPose,
                        const Position& endPos,
                        const UAVOrientation& endPose,
                        const QList<QPolygonF>& obstacles);
    virtual ~IntermediatePlanner();

    virtual bool plan()=0;
    virtual QList<Position> results() const=0;

    const UAVParameters& uavParams() const;

    const Position& startPos() const;
    const Position& endPos() const;

    const UAVOrientation& startPose() const;
    const UAVOrientation& endPose() const;

    const QList<QPolygonF>& obstacles() const;

private:
    const UAVParameters& _uavParams;
    const Position& _startPos;
    const UAVOrientation& _startPose;
    const Position& _endPos;
    const UAVOrientation& _endPose;
    const QList<QPolygonF>& _obstacles;
};

#endif // INTERMEDIATEPLANNER_H
