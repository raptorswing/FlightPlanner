#include "WaysetManager.h"

#include <QtDebug>

WaysetManager::WaysetManager(MapGraphicsScene *scene, QObject *parent) :
    QObject(parent), _scene(scene)
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

//public slot
void WaysetManager::appendWaypoint(Position pos)
{
    Waypoint * wpt = new Waypoint();
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
