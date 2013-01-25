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
    void _buildSchedule();

    QList<QSharedPointer<FlightTask> > _tasks;
    QHash<QSharedPointer<FlightTask>, QSharedPointer<FlightTaskArea> > _tasks2areas;
    QHash<QSharedPointer<FlightTaskArea>, Position> _areaStartPositions;
    QHash<QSharedPointer<FlightTaskArea>, Position> _areaEndPositions;
    QHash<QSharedPointer<FlightTaskArea>, UAVOrientation> _areaStartOrientations;
    QHash<QSharedPointer<FlightTaskArea>, UAVOrientation> _areaEndOrientations;
    QHash<QSharedPointer<FlightTask>, QList<Position> > _taskSubFlights;
    QHash<QSharedPointer<FlightTask>, QList<Position> > _startTransitionSubFlights;
    
};

#endif // HIERARCHICALPLANNER_H
