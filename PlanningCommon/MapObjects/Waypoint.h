#ifndef WAYPOINT_H
#define WAYPOINT_H

#include <QPointer>
#include <QWeakPointer>

#include "MapGraphicsObject.h"
#include "PlanningProblem.h"
#include "UAVOrientation.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT Waypoint : public MapGraphicsObject
{
    Q_OBJECT
public:
    enum WaypointLineMode
    {
        DubinLineMode,
        StraightLineMode
    };

public:
    Waypoint(const QWeakPointer<PlanningProblem>& problem,
             Waypoint * prev = 0,
             Waypoint * next = 0,
             WaypointLineMode lMode = DubinLineMode);
    Waypoint(const QWeakPointer<PlanningProblem>& problem,
             const UAVOrientation& angle,
             Waypoint * prev = 0,
             Waypoint * next = 0,
             WaypointLineMode lMode = DubinLineMode);
    ~Waypoint();

    Waypoint * prev() const;
    Waypoint * next() const;

    int autoFixKinematicsIteration();
    int autoFixDistancesIteration();

    //virtual from MapGraphicsObject
    virtual void setPos(const QPointF&);

    //pure-virtual from MapGraphicsObject
    QRectF boundingRect() const;

    //pure-virtual from MapGraphicsObject
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    UAVOrientation angle() const;
    
signals:
    void newWaypointGenerated(Waypoint * newOne);
    void aboutToDelete(Waypoint * next);
    
public slots:
    void setPrev(Waypoint * nPrev);
    void setNext(Waypoint * nNext);

    void append(Waypoint * newWpt);
    void insertNewAfter();
    void insertNewBefore();
    void deleteAllFollowing();
    void deleteAllPrevious();

protected:
    //virtual from MapGraphicsObject
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);

private slots:
    void updateLine();

private:
    static qreal angleBetween(const QPointF& src, const QPointF& dst);
    static qreal angleAbsVal(qreal angleA, qreal angleB);
    static qreal normalizeAngle(qreal angle);
    static QPointF perturbPosition(const QPointF& pos);

    qreal _displaySize;
    QPointer<Waypoint> _prev;
    QPointer<Waypoint> _next;

    QWeakPointer<PlanningProblem> _problem;

     MapGraphicsObject *_lineObj;
    const WaypointLineMode _lMode;

    bool _manualAngleSet;
    UAVOrientation _manualAngle;
    
};

#endif // WAYPOINT_H
