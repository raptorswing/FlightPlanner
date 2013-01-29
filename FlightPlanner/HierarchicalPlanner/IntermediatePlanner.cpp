#include "IntermediatePlanner.h"

IntermediatePlanner::IntermediatePlanner(const Position &startPos,
                                         const UAVOrientation &startPose,
                                         const Position &endPos,
                                         const UAVOrientation &endPose,
                                         const QList<QPolygonF> &obstacles) :
    _startPos(startPos), _startPose(startPose), _endPos(endPos), _endPose(endPose), _obstacles(obstacles)
{
}

IntermediatePlanner::~IntermediatePlanner()
{
}

const Position &IntermediatePlanner::startPos() const
{
    return _startPos;
}

const Position &IntermediatePlanner::endPos() const
{
    return _endPos;
}

const UAVOrientation &IntermediatePlanner::startPose() const
{
    return _startPose;
}

const UAVOrientation &IntermediatePlanner::endPose() const
{
    return _endPose;
}

const QList<QPolygonF> &IntermediatePlanner::obstacles() const
{
    return _obstacles;
}
