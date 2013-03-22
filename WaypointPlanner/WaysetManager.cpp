#include "WaysetManager.h"

#include <QtDebug>
#include <QTimer>

WaysetManager::WaysetManager(MapGraphicsScene *scene,
                             QSharedPointer<PlanningProblem> problem,
                             QObject *parent) :
    QObject(parent), _scene(scene), _problem(problem)
{
}

QList<Position> WaysetManager::currentWayset() const
{
    QList<Position> toRet;

    QPointer<Waypoint> current = _first;
    while (!current.isNull())
    {
        toRet.append(Position(current->pos()));
        current = current->next();
    }

    return toRet;
}

void WaysetManager::setWayset(const QList<Position> &wayset)
{
    if (!_first.isNull())
    {
        _first->deleteLater();
        _first = 0;
    }

    foreach(const Position& pos, wayset)
    {
        this->appendWaypoint(pos);
    }
}

//public slot
void WaysetManager::appendWaypoint(Position pos)
{
    Waypoint * wpt = new Waypoint(_problem);
    wpt->setPos(pos.lonLat());
    _scene->addObject(wpt);

    if (_first.isNull())
        this->setNewFirst(wpt);
    else
        _first->append(wpt);

    this->handleNewWaypoint(wpt);
}

//public slot
void WaysetManager::enableMouseInteraction(bool enable)
{
    QPointer<Waypoint> current = _first;
    _mouseInteraction = enable;

    while (!current.isNull())
    {
        current->setFlag(MapGraphicsObject::ObjectIsSelectable, enable);
        current->setFlag(MapGraphicsObject::ObjectIsMovable, enable);
        current = current->next();
    }
}

//public slot
void WaysetManager::fixKinematics()
{
    if (_first.isNull())
        return;

    int numErrors = _first->autoFixKinematicsIteration();

    if (numErrors > 0)
        QTimer::singleShot(1, this, SLOT(fixKinematics()));
}

//public slot
void WaysetManager::fixDistances()
{
    if (_first.isNull())
        return;

    int numErrors = _first->autoFixDistancesIteration();

    if (numErrors > 0)
        QTimer::singleShot(1, this, SLOT(fixDistances()));
}

//public slot
void WaysetManager::fixAll(bool repeatable)
{
    if (_first.isNull())
        return;

    int numKinematics = _first->autoFixKinematicsIteration();
    int numDistance = _first->autoFixDistancesIteration();

    if (numKinematics > 0 || numDistance > 0)
        QTimer::singleShot(1, this, SLOT(fixAll()));
    else if (repeatable)
        this->fixAll(false);
}

//private slot
void WaysetManager::setNewFirst(Waypoint *wpt)
{
    _first = wpt;
    if (_first.isNull())
        return;

    connect(wpt,
            SIGNAL(aboutToDelete(Waypoint*)),
            this,
            SLOT(setNewFirst(Waypoint*)));
}

//private slot
void WaysetManager::handleNewWaypoint(Waypoint *wpt)
{
    if (!wpt)
        return;

    connect(wpt,
            SIGNAL(newWaypointGenerated(Waypoint*)),
            this,
            SLOT(handleNewWaypoint(Waypoint*)));

    wpt->setFlag(MapGraphicsObject::ObjectIsSelectable, _mouseInteraction);
    wpt->setFlag(MapGraphicsObject::ObjectIsMovable, _mouseInteraction);
}
