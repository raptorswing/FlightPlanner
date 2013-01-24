#include "FlightPlanner.h"

FlightPlanner::FlightPlanner(QSharedPointer<PlanningProblem> prob,
                             QObject *parent) :
    QObject(parent), _prob(prob), _iterations(0)
{
    _planningTimer = new QTimer(this);
    connect(_planningTimer,
            SIGNAL(timeout()),
            this,
            SLOT(handlePlanningTimerTimeout()));
}

FlightPlanner::~FlightPlanner()
{
    this->pausePlanning();
    _prob.clear();
}

QSharedPointer<PlanningProblem> FlightPlanner::problem() const
{
    return _prob;
}

void FlightPlanner::setProblem(QSharedPointer<PlanningProblem> nProb)
{
    _prob = nProb;
    this->resetPlanning();
}

Fitness FlightPlanner::bestFitnessSoFar() const
{
    return _bestFitnessSoFar;
}

const QList<Position> &FlightPlanner::bestFlightSoFar() const
{
    return _bestFlightSoFar;
}

quint32 FlightPlanner::iterations() const
{
    return _iterations;
}

//public slot
void FlightPlanner::startPlanning()
{
    if (_prob.isNull())
    {
        qDebug() << "Can't start planning - problem is null";
        return;
    }

    if (!_prob->startingPositionDefined() || !_prob->startingOrientationDefined())
    {
        qDebug() << "Can't start planning - no start position";
        return;
    }

    //Have the concrete implementation do its setup
    this->doStart();

    _planningTimer->start(0);
    this->plannerStatusChanged(Running);
}

//public slot
void FlightPlanner::pausePlanning()
{
    _planningTimer->stop();
    this->plannerStatusChanged(Paused);
}

//public slot
void FlightPlanner::resetPlanning()
{
    this->pausePlanning();
    this->doReset();

    _bestFitnessSoFar = Fitness();
    _bestFlightSoFar.clear();
    _iterations = 0;


    this->plannerProgressChanged(0.0,0);
    this->plannerStatusChanged(Stopped);
}

//protected
void FlightPlanner::setBestFitnessSoFar(const Fitness &nBest)
{
    _bestFitnessSoFar = nBest;
}

//protected
void FlightPlanner::setBestFlightSoFar(const QList<Position> &nFlight)
{
    _bestFlightSoFar = nFlight;
}

//private slot
//The timer calls this one, which will call the polymorphic instance
void FlightPlanner::handlePlanningTimerTimeout()
{
    //Have the concrete implementation do its thing
    this->doIteration();

    //Then update anyone watching us with the number of iterations we've done and the best fitness so far
    this->plannerProgressChanged(this->bestFitnessSoFar().combined(),
                                  ++_iterations);
}
