#ifndef DUMMYPLANNER_H
#define DUMMYPLANNER_H

#include <QObject>

#include "FlightPlanner.h"

class DummyPlanner : public FlightPlanner
{
    Q_OBJECT
public:
    explicit DummyPlanner(QSharedPointer<PlanningProblem> prob = QSharedPointer<PlanningProblem>(),
                          QObject *parent = 0);

protected:
    //pure-virtual from FlightPlanner
    virtual void doStart();
    virtual void doIteration();
    virtual void doReset();
    
};

#endif // DUMMYPLANNER_H
