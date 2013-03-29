#ifndef INTERMEDIATEPLANNER_H
#define INTERMEDIATEPLANNER_H

#include "Position.h"
#include "UAVOrientation.h"
#include "UAVParameters.h"
#include "Wayset.h"

#include <QList>
#include <QPolygonF>

class IntermediatePlanner
{
public:
    IntermediatePlanner(const UAVParameters& uavParams,
                        const Position& startPos,
                        const UAVOrientation& startAngle,
                        const Position& endPos,
                        const UAVOrientation& endAngle,
                        const QList<QPolygonF>& obstacles);
    virtual ~IntermediatePlanner();

    virtual bool plan()=0;
    virtual Wayset results() const=0;

    const UAVParameters& uavParams() const;

    const Position& startPos() const;
    const Position& endPos() const;

    const UAVOrientation& startAngle() const;
    const UAVOrientation& endAngle() const;

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
