#include "FlightTaskArea.h"

#include <QtDebug>

FlightTaskArea::FlightTaskArea()
{
}

FlightTaskArea::FlightTaskArea(const QPolygonF &geoPoly) :
    _geoPoly(geoPoly)
{
}

const QPolygonF &FlightTaskArea::geoPoly() const
{
    return _geoPoly;
}

void FlightTaskArea::setGeoPoly(const QPolygonF &nPoly)
{
    _geoPoly = nPoly;

    this->geoPolyChanged(_geoPoly);
    this->flightTaskAreaChanged();
}

const QSet<QSharedPointer<FlightTask> > &FlightTaskArea::tasks() const
{
    return _tasks;
}

void FlightTaskArea::addTask(QSharedPointer<FlightTask> task)
{
    if (_tasks.contains(task) && !task.isNull())
        return;

    _tasks.insert(task);

    this->taskAdded(task);
    this->flightTaskAreaChanged();
}

void FlightTaskArea::removeTask(QSharedPointer<FlightTask> task)
{
    if (!_tasks.contains(task) || task.isNull())
        return;

    _tasks.remove(task);

    this->taskRemoved(task);
    this->flightTaskAreaChanged();
}

int FlightTaskArea::numTasks() const
{
    return _tasks.size();
}
