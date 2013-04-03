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

    const QSet<int> selectedIndices() const;

    void setPlanningProblem(const QSharedPointer<PlanningProblem>& problem);
    
signals:
    void waysetChanged();
    void waysetSelectionsChanged(QSet<int> indices);
    
public slots:
    void appendWaypoint(const Position& pos);
    void appendWaypoint(const Position& pos, const UAVOrientation& orientation);
    void enableMouseInteraction(bool enable=true);
    void fixKinematics();
    void fixDistances();
    void fixAll(bool repeatable = true);

private slots:
    void setNewFirst(Waypoint * wpt);
    void handleNewWaypoint(Waypoint * obj);
    void handleSelectionChanged();

private:
    MapGraphicsScene * _scene;
    QWeakPointer<PlanningProblem> _problem;
    Waypoint::WaypointLineMode _lineMode;

    QPointer<Waypoint> _first;
    bool _mouseInteraction;

    QSet<int> _selectedIndices;
    
};

#endif // WAYSETDISPLAYMANAGER_H
