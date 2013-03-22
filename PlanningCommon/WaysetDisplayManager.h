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
    explicit WaysetDisplayManager(MapGraphicsScene * scene,
                                  QSharedPointer<PlanningProblem> problem,
                                  QObject *parent = 0);

    QList<Position> wayset() const;
    void setWayset(const QList<Position>& wayset);
    
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

    QPointer<Waypoint> _first;
    bool _mouseInteraction;
    
};

#endif // WAYSETDISPLAYMANAGER_H
