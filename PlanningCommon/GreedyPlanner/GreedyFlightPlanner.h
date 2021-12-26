#ifndef GREEDYFLIGHTPLANNER_H
#define GREEDYFLIGHTPLANNER_H

#include "FlightPlanner.h"
#include "GreedyPlanningNode.h"

#include <QMap>
#include <QSharedPointer>
#include <QQueue>

class GreedyFlightPlanner : public FlightPlanner
{
    Q_OBJECT
public:
    explicit GreedyFlightPlanner(QSharedPointer<PlanningProblem> prob = QSharedPointer<PlanningProblem>(),
                                 QObject *parent = 0);
    
signals:
    
public slots:

protected:
    //pure-virtual from FlightPlanner
    virtual void doStart();

    //pure-virtual from FlightPlanner
    virtual void doIteration();

    //pure-virtual from FlightPlanner
    virtual void doReset();

private:
    //QMap<qreal, QSharedPointer<GreedyPlanningNode> > _frontier;
    QQueue<QSharedPointer<GreedyPlanningNode> > _frontier;

    qreal _lastOrientation;

    Fitness _bestFitnessThisIteration;
    Wayset _bestPathThisIteration;
};

#endif // GREEDYFLIGHTPLANNER_H
