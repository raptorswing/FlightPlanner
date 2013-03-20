#ifndef WAYSETMANAGER_H
#define WAYSETMANAGER_H

#include <QObject>
#include <QList>
#include <QPointer>

#include "Position.h"
#include "MapGraphicsScene.h"
#include "Waypoint.h"

class WaysetManager : public QObject
{
    Q_OBJECT
public:
    explicit WaysetManager(MapGraphicsScene * scene,
                           QObject *parent = 0);

    QList<Position> currentWayset() const;
    
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

    QPointer<Waypoint> _first;
    bool _mouseInteraction;
    
};

#endif // WAYSETMANAGER_H
