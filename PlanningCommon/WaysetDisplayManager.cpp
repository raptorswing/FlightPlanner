#include "WaysetDisplayManager.h"

#include <QtDebug>
#include <QTimer>

WaysetDisplayManager::WaysetDisplayManager(MapGraphicsScene *scene,
                                           QSharedPointer<PlanningProblem> problem,
                                           Waypoint::WaypointLineMode lineMode,
                                           QObject *parent) :
    QObject(parent), _scene(scene), _problem(problem), _lineMode(lineMode)
{
}

Wayset WaysetDisplayManager::wayset() const
{
    Wayset toRet;

    QPointer<Waypoint> current = _first;
    while (!current.isNull())
    {
        toRet.append(Position(current->pos()),
                     current->angle());
        current = current->next();
    }

    return toRet;
}

void WaysetDisplayManager::setWayset(const Wayset &wayset,
                                     Waypoint::WaypointLineMode lMode)
{
    _lineMode = lMode;

    if (!_first.isNull())
    {
        disconnect(_first.data(),
                   SIGNAL(aboutToDelete(Waypoint*)),
                   this,
                   SLOT(setNewFirst(Waypoint*)));
    }

    QPointer<Waypoint> current = _first;
    while (!current.isNull())
    {
        current->deleteLater();
        current = current->next();
    }
    _first = 0;

    foreach(const UAVPose& pose, wayset.poses())
        this->appendWaypoint(pose.pos(), pose.angle());
}

const QSet<int> WaysetDisplayManager::selectedIndices() const
{
    return _selectedIndices;
}

void WaysetDisplayManager::setPlanningProblem(const QSharedPointer<PlanningProblem> &problem)
{
    _problem = problem.toWeakRef();

    this->waysetChanged();
}

Waypoint::WaypointLineMode WaysetDisplayManager::lineMode() const
{
    return _lineMode;
}

//public slot
void WaysetDisplayManager::appendWaypoint(const Position& pos)
{
    Waypoint * wpt = new Waypoint(_problem, 0, 0, _lineMode);
    wpt->setPos(pos.lonLat());
    _scene->addObject(wpt);

    if (_first.isNull())
        this->setNewFirst(wpt);
    else
        _first->append(wpt);

    this->handleNewWaypoint(wpt);
}

//public slot
void WaysetDisplayManager::appendWaypoint(const Position &pos, const UAVOrientation &orientation)
{
    Waypoint * wpt = new Waypoint(_problem, orientation, 0, 0, _lineMode);
    wpt->setPos(pos.lonLat());
    _scene->addObject(wpt);

    if (_first.isNull())
        this->setNewFirst(wpt);
    else
        _first->append(wpt);

    this->handleNewWaypoint(wpt);
}

//public slot
void WaysetDisplayManager::enableMouseInteraction(bool enable)
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
void WaysetDisplayManager::fixKinematics()
{
    if (_first.isNull())
        return;

    int numErrors = _first->autoFixKinematicsIteration();

    if (numErrors > 0)
        QTimer::singleShot(1, this, SLOT(fixKinematics()));
}

//public slot
void WaysetDisplayManager::fixDistances()
{
    if (_first.isNull())
        return;

    int numErrors = _first->autoFixDistancesIteration();

    if (numErrors > 0)
        QTimer::singleShot(1, this, SLOT(fixDistances()));
}

//public slot
void WaysetDisplayManager::fixAll(bool repeatable)
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
void WaysetDisplayManager::setNewFirst(Waypoint *wpt)
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
void WaysetDisplayManager::handleNewWaypoint(Waypoint *wpt)
{
    if (!wpt)
        return;

    connect(wpt,
            SIGNAL(newWaypointGenerated(Waypoint*)),
            this,
            SLOT(handleNewWaypoint(Waypoint*)));

    connect(wpt,
            SIGNAL(destroyed()),
            this,
            SIGNAL(waysetChanged()));
    connect(wpt,
            SIGNAL(destroyed()),
            this,
            SLOT(handleSelectionChanged()));

    connect(wpt,
            SIGNAL(posChanged()),
            this,
            SIGNAL(waysetChanged()));

    connect(wpt,
            SIGNAL(selectedChanged()),
            this,
            SLOT(handleSelectionChanged()));

    wpt->setFlag(MapGraphicsObject::ObjectIsSelectable, _mouseInteraction);
    wpt->setFlag(MapGraphicsObject::ObjectIsMovable, _mouseInteraction);

    QTimer::singleShot(0, this, SIGNAL(waysetChanged()));
    QTimer::singleShot(0, this, SLOT(handleSelectionChanged()));
}

//private slot
void WaysetDisplayManager::handleSelectionChanged()
{
    QSet<int> indices;

    QPointer<Waypoint> current = _first;

    int count = 0;
    while (!current.isNull())
    {
        if (current->isSelected())
            indices.insert(count);
        count++;

        current = current->next();
    }

    _selectedIndices = indices;
    this->waysetSelectionsChanged(indices);
}
