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

const QList<QSharedPointer<FlightTask> > &FlightTaskArea::tasks() const
{
    return _tasks;
}

void FlightTaskArea::addTask(QSharedPointer<FlightTask> task)
{
    if (_tasks.contains(task) && !task.isNull())
        return;

    this->taskAboutToAdd();

    _tasks.append(task);

    this->taskAdded(task);
    this->flightTaskAreaChanged();

    connect(task.data(),
            SIGNAL(flightTaskChanged()),
            this,
            SIGNAL(flightTaskAreaChanged()));
}

void FlightTaskArea::removeTask(QSharedPointer<FlightTask> task)
{
    if (task.isNull())
        return;

    const int index = _tasks.indexOf(task);
    if (index <= -1)
        return;

    this->taskAboutToRemove(index);
    _tasks.removeOne(task);
    this->taskRemoved(task);

    this->flightTaskAreaChanged();
}

int FlightTaskArea::numTasks() const
{
    return _tasks.size();
}
