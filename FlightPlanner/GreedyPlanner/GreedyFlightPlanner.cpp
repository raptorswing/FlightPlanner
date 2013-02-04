#include "GreedyFlightPlanner.h"

#include <QtDebug>
#include <QMutableMapIterator>
#include <cmath>

#include "guts/Conversions.h"

const int GREED_DEPTH = 7;

GreedyFlightPlanner::GreedyFlightPlanner(QSharedPointer<PlanningProblem> prob, QObject *parent) :
    FlightPlanner(prob, parent)
{
}

//protected
//pure-virtual from FlightPlanner
void GreedyFlightPlanner::doStart()
{
    _frontier.clear();

    QSharedPointer<GreedyPlanningNode> topNode(new GreedyPlanningNode(this->problem()->startingPosition(),
                                                                      this->problem()->startingOrientation(),
                                                                      0,
                                                                      GREED_DEPTH));
    //_frontier.insert(0, topNode);
    _frontier.enqueue(topNode);
}

//protected
//pure-virtual from FlightPlanner
void GreedyFlightPlanner::doIteration()
{
    if (_frontier.isEmpty())
    {
        UAVOrientation lastOrientation;
        Position lastPos = _bestPathThisIteration.last();
        if (this->bestFlightSoFar().size() >= 2)
        {
            Position secondLast = this->bestFlightSoFar().at(this->bestFlightSoFar().size()-2);
            QVector3D enu = Conversions::lla2enu(lastPos,secondLast);
            lastOrientation.setRadians(_lastOrientation);
        }
        QSharedPointer<GreedyPlanningNode> iterTopNode(new GreedyPlanningNode(lastPos,
                                                                              lastOrientation,
                                                                              0,
                                                                              GREED_DEPTH));
        iterTopNode->setFlighPath(this->bestFlightSoFar());
        _frontier.enqueue(iterTopNode);
    }

    _bestFitnessThisIteration = Fitness(0, 0);
    _bestPathThisIteration.clear();

    while (!_frontier.isEmpty())
    {
        QSharedPointer<GreedyPlanningNode> top = _frontier.dequeue();

        //Check out the performance of the top's flight
        const QList<Position>& flightPath = top->flightPath();

        Fitness score = this->problem()->calculateFlightPerformance(flightPath);


        if (score >= this->bestFitnessSoFar())
        {
            this->setBestFitnessSoFar(score);
            this->setBestFlightSoFar(flightPath);
            _lastOrientation = top->orientation().radians();
        }

        if (score >= _bestFitnessThisIteration)
        {
            _bestFitnessThisIteration = score;
            _bestPathThisIteration = flightPath;
        }

        QVector<QSharedPointer<GreedyPlanningNode> > children = top->visit();
        foreach(QSharedPointer<GreedyPlanningNode> child, children)
        {
            child->setParent(top);
            _frontier.enqueue(child);
        }
    }
}

//protected
//pure-virtual from FlightPlanner
void GreedyFlightPlanner::doReset()
{
    _frontier.clear();
}
