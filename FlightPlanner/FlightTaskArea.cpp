#include "FlightTaskArea.h"

#include <QtDebug>

#include "FlightTasks/CoverageTask.h"
#include "FlightTasks/FlyThroughTask.h"
#include "FlightTasks/NoFlyFlightTask.h"
#include "FlightTasks/SamplingTask.h"\

FlightTaskArea::FlightTaskArea()
{
    this->setAreaName("Untitled");
}

FlightTaskArea::FlightTaskArea(const QPolygonF &geoPoly) :
    _geoPoly(geoPoly)
{
}

//for de-serializing
FlightTaskArea::FlightTaskArea(QDataStream &stream)
{
    stream >> _geoPoly;
    stream >> _areaName;

    int numTasks;
    stream >> numTasks;
    for (int i = 0; i < numTasks; i++)
    {
        QString sKey;
        stream >> sKey;

        QSharedPointer<FlightTask> task;
        if (sKey == "CoverageTask")
            task = QSharedPointer<FlightTask>(new CoverageTask(stream));
        else if (sKey == "FlyThroughTask")
            task = QSharedPointer<FlightTask>(new FlyThroughTask(stream));
        else if (sKey == "SamplingTask")
            task = QSharedPointer<SamplingTask>(new SamplingTask(stream));
        else if (sKey == "NoFlyFlightTask")
            task = QSharedPointer<NoFlyFlightTask>(new NoFlyFlightTask(stream));
        else
        {
            qWarning() << "Unknown task" << sKey << " can't deserialize";
            continue;
        }
        FlightTask::_uuidToWeakTask.insert(task->uuid(), task.toWeakRef()); //For inter-task dependencies
        _tasks.append(task);
    }
}

//pure-virtual from Serializable
QString FlightTaskArea::serializationType() const
{
    return "FlightTaskArea";
}

//pure-virtual from Serializable
void FlightTaskArea::serialize(QDataStream &stream) const
{
    stream << _geoPoly;
    stream << _areaName;

    stream << this->tasks().size();
    foreach(const QSharedPointer<FlightTask>& task, this->tasks())
    {
        stream << task->serializationType();
        task->serialize(stream);
    }
}

const QPolygonF &FlightTaskArea::geoPoly() const
{
    return _geoPoly;
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

const QString &FlightTaskArea::areaName() const
{
    return _areaName;
}

//public slot
void FlightTaskArea::setGeoPoly(const QPolygonF &nPoly)
{
    _geoPoly = nPoly;

    this->geoPolyChanged(_geoPoly);
    this->flightTaskAreaChanged();
}

//public slot
void FlightTaskArea::setAreaName(const QString &nName)
{
    _areaName = nName;
    this->flightTaskAreaNameChanged();
    this->flightTaskAreaChanged();
}
