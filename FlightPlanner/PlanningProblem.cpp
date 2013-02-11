#include "PlanningProblem.h"

#include <QtDebug>

PlanningProblem::PlanningProblem() :
    _startingOrientationDefined(false), _startingPositionDefined(false)
{
}

bool PlanningProblem::startingOrientationDefined() const
{
    return _startingOrientationDefined;
}

const UAVOrientation &PlanningProblem::startingOrientation() const
{
    return _startingOrientation;
}

void PlanningProblem::setStartingOrientation(const UAVOrientation &orientation)
{
    _startingOrientation = orientation;
    _startingOrientationDefined = true;

    this->startingOrientationChanged(_startingOrientation);
    this->planningProblemChanged();
}

bool PlanningProblem::startingPositionDefined() const
{
    return _startingPositionDefined;
}

const Position &PlanningProblem::startingPosition() const
{
    return _startingPosition;
}

void PlanningProblem::setStartingPosition(const Position &pos)
{
    _startingPosition = pos;
    _startingPositionDefined = true;

    this->startingPositionChanged(_startingPosition);
    this->planningProblemChanged();
}

void PlanningProblem::addTaskArea(QSharedPointer<FlightTaskArea> area)
{
    if (area.isNull())
        return;

    _areas.insert(area);

    connect(area.data(),
            SIGNAL(flightTaskAreaChanged()),
            this,
            SIGNAL(planningProblemChanged()));

    this->flighTaskAreaAdded(area);
    this->planningProblemChanged();
}

void PlanningProblem::addTaskArea(const QPointF &centerPos)
{
    QSharedPointer<FlightTaskArea> area(new FlightTaskArea());

    qreal delta = 0.001;
    QPolygonF poly;
    poly << (centerPos + QPointF(delta,delta));
    poly << (centerPos + QPointF(-delta,delta));
    poly << (centerPos + QPointF(-delta,-delta));
    poly << (centerPos + QPointF(delta,-delta));
    area->setGeoPoly(poly);

    this->addTaskArea(area);
}

void PlanningProblem::removeTaskArea(QSharedPointer<FlightTaskArea> area)
{
    _areas.remove(area);

    this->flightTaskAreaRemoved(area);
    this->planningProblemChanged();
}

Fitness PlanningProblem::calculateFlightPerformance(const QList<Position> &positions) const
{
    qreal taskScore = 0.0;
    qreal efficiencyScore = 0.0;

    foreach(const QSharedPointer<FlightTaskArea>& area, _areas)
    {
        const QPolygonF& geoPoly = area->geoPoly();
        foreach(const QSharedPointer<FlightTask>& task, area->tasks())
        {
            const qreal subScore = task->calculateFlightPerformance(positions, geoPoly);
            if (task->shortnessRewardApplies() && subScore >= task->maxTaskPerformance())
                efficiencyScore += subScore / positions.size();
            taskScore += subScore;
        }
    }

    return Fitness(taskScore, efficiencyScore);
}

const QSet<QSharedPointer<FlightTaskArea> > &PlanningProblem::areas() const
{
    return _areas;
}

const UAVParameters &PlanningProblem::uavParameters() const
{
    return _uavParameters;
}

void PlanningProblem::setUAVParameters(const UAVParameters &nParams)
{
    _uavParameters = nParams;

    this->planningProblemChanged();
}
