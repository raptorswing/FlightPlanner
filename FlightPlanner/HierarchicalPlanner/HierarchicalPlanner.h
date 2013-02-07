#ifndef HIERARCHICALPLANNER_H
#define HIERARCHICALPLANNER_H

#include <QObject>
#include <QList>
#include <QHash>

#include "FlightPlanner.h"
#include "PlanningProblem.h"
#include "Position.h"

class HierarchicalPlanner : public FlightPlanner
{
    Q_OBJECT
public:
    explicit HierarchicalPlanner(QSharedPointer<PlanningProblem> prob = QSharedPointer<PlanningProblem>(),
                                 QObject *parent = 0);

protected:
    //pure-virtual from FlightPlanner
    virtual void doStart();
    virtual void doIteration();
    virtual void doReset();

private:
    void _buildStartAndEndPositions();
    void _buildStartTransitions();
    void _buildSubFlights();
    bool _buildSchedule();

    bool _interpolatePath(const QList<Position>& path,
                              const UAVOrientation& startingOrientation,
                              qreal time,
                              Position * outPosition,
                              UAVOrientation * outOrientation) const;

    QList<Position> _generateTransitionFlight(const Position& startPos,
                                              const UAVOrientation& startPose,
                                              const Position& endPos,
                                              const UAVOrientation& endPose);

    QList<Position> _getPathPortion(const QList<Position>& path,
                                    qreal portionStartTime,
                                    qreal portionEndTime) const;

    QList<QSharedPointer<FlightTask> > _tasks;
    QHash<QSharedPointer<FlightTask>, QSharedPointer<FlightTaskArea> > _tasks2areas;
    QHash<QSharedPointer<FlightTaskArea>, Position> _areaStartPositions;
    QHash<QSharedPointer<FlightTaskArea>, UAVOrientation> _areaStartOrientations;
    QHash<QSharedPointer<FlightTask>, QList<Position> > _taskSubFlights;
    QHash<QSharedPointer<FlightTaskArea>, QList<Position> > _startTransitionSubFlights;
    QList<QPolygonF> _obstacles;
    
};

#endif // HIERARCHICALPLANNER_H
