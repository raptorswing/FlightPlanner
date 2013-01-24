#include "DummyPlanner.h"

DummyPlanner::DummyPlanner(QSharedPointer<PlanningProblem> prob,
                           QObject *parent) :
    FlightPlanner(prob, parent)
{
}

//protected
//pure-virtual from FlightPlanner
void DummyPlanner::doReset()
{
}

//protected
//pure-virtual from FlightPlanner
void DummyPlanner::doIteration()
{
}

//protected
//pure-virtual from FlightPlanner
void DummyPlanner::doStart()
{
}
