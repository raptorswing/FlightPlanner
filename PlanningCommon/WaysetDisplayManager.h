#ifndef WAYSETDISPLAYMANAGER_H
#define WAYSETDISPLAYMANAGER_H

#include <QObject>
#include <QList>
#include <QPointer>

#include "Position.h"
#include "MapGraphicsScene.h"
#include "MapObjects/Waypoint.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT WaysetDisplayManager : public QObject
{
    Q_OBJECT
public:
    WaysetDisplayManager(MapGraphicsScene * scene,
                         QSharedPointer<PlanningProblem> problem,
                         Waypoint::WaypointLineMode lineMode = Waypoint::DubinLineMode,
                         QObject *parent = 0);

    Wayset wayset() const;
    void setWayset(const Wayset& wayset, Waypoint::WaypointLineMode lMode = Waypoint::DubinLineMode);

    void setPlanningProblem(const QSharedPointer<PlanningProblem>& problem);
    
signals:
    
public slots:
    void appendWaypoint(Position pos);
    void enableMouseInteraction(bool enable=true);
    void fixKinematics();
    void fixDistances();
    void fixAll(bool repeatable = true);

private slots:
    void setNewFirst(Waypoint * wpt);
    void handleNewWaypoint(Waypoint * obj);

private:
    MapGraphicsScene * _scene;
    QWeakPointer<PlanningProblem> _problem;
    Waypoint::WaypointLineMode _lineMode;

    QPointer<Waypoint> _first;
    bool _mouseInteraction;
    
};

#endif // WAYSETDISPLAYMANAGER_H
