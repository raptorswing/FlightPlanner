#ifndef SMARTINTERMEDIATEPLANNER_H
#define SMARTINTERMEDIATEPLANNER_H

#include "IntermediatePlanner.h"

class SmartIntermediatePlanner : public IntermediatePlanner
{
public:
    SmartIntermediatePlanner(const UAVParameters& uavParams,
                             const Position& startPos,
                             const UAVOrientation& startAngle,
                             const Position& endPos,
                             const UAVOrientation& endAngle,
                             const QList<QPolygonF>& obstacles);

    void setAstarGranularity(qreal granularity);

    //pure-virtual from IntermediatePlanner
    virtual bool plan();

    //pure-virtual from IntermediatePlanner
    virtual Wayset results() const;

private:
    Wayset _results;

    bool _customGranularity;
    qreal _granularity;
};

#endif // SMARTINTERMEDIATEPLANNER_H
