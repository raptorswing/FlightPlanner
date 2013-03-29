#include "PlanningProblem.h"

#include <QtDebug>

PlanningProblem::PlanningProblem() :
    _startingOrientationDefined(false), _startingPositionDefined(false)
{
}

//for de-serializing
PlanningProblem::PlanningProblem(QDataStream &stream)
{
    stream >> _startingOrientationDefined;
    if (_startingOrientationDefined)
        _startingOrientation = UAVOrientation(stream);

    stream >> _startingPositionDefined;
    if (_startingPositionDefined)
        stream >> _startingPosition;

    int numAreas;
    stream >> numAreas;

    for (int i = 0; i < numAreas; i++)
    {
        QSharedPointer<FlightTaskArea> area(new FlightTaskArea(stream));
        this->addTaskArea(area);
    }

    //Resolve dependencies between flight tasks. This requires a "second pass" here.
    foreach(const QSharedPointer<FlightTaskArea>& area, _areas)
    {
        foreach(const QSharedPointer<FlightTask>& task, area->tasks())
            task->resolveDependencies();
    }
    FlightTask::_uuidToWeakTask.clear();

    stream >> _uavParameters;
}

//pure-virtual from Serializable
QString PlanningProblem::serializationType() const
{
    return "PlanningProblem";
}

//pure-virtual from Serializable
void PlanningProblem::serialize(QDataStream &stream) const
{
    stream << _startingOrientationDefined;
    if (_startingOrientationDefined)
        _startingOrientation.serialize(stream);

    stream << _startingPositionDefined;
    /* Position is from MapGraphics and doesn't inherit Serializable.
     Luckily, it has the normal streaming operators we can use
    */
    if (_startingPositionDefined)
        stream << _startingPosition;

    stream << this->areas().size();
    foreach(const QSharedPointer<FlightTaskArea>& area, this->areas())
        area->serialize(stream);

    stream << _uavParameters;
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

Fitness PlanningProblem::calculateFlightPerformance(const Wayset &positions) const
{
    qreal taskScore = 0.0;
    qreal efficiencyScore = 0.0;

    foreach(const QSharedPointer<FlightTaskArea>& area, _areas)
    {
        const QPolygonF& geoPoly = area->geoPoly();
        foreach(const QSharedPointer<FlightTask>& task, area->tasks())
        {
            const qreal subScore = task->calculateFlightPerformance(positions, geoPoly, _uavParameters);
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

    this->uavParametersChanged(nParams);
    this->planningProblemChanged();
}
