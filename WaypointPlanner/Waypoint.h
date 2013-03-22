#ifndef WAYPOINT_H
#define WAYPOINT_H

#include <QPointer>

#include "DubinsLineObject.h"
#include "MapGraphicsObject.h"

class Waypoint : public MapGraphicsObject
{
    Q_OBJECT
public:
    Waypoint(Waypoint * prev = 0,
             Waypoint * next = 0);
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

    DubinsLineObject *_lineObj;
    
};

#endif // WAYPOINT_H
