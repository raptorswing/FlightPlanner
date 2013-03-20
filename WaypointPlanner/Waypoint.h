#ifndef WAYPOINT_H
#define WAYPOINT_H

#include "CircleObject.h"
#include "LineObject.h"

class Waypoint : public CircleObject
{
    Q_OBJECT
public:
    Waypoint(Waypoint * prev = 0,
             Waypoint * next = 0);
    ~Waypoint();

    Waypoint * prev() const;
    Waypoint * next() const;

    //virtual from CircleObject
    virtual void setPos(const QPointF&);
    
signals:
    void newWaypointGenerated(Waypoint * newOne);
    void aboutToDelete(Waypoint * next);
    
public slots:
    void setPrev(Waypoint * nPrev);
    void setNext(Waypoint * nNext);

    void append(Waypoint * newWpt);
    void insertNewAfter();
    void insertNewBefore();

protected:
    //virtual from MapGraphicsObject
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);

private slots:
    void updateLine();

private:
    Waypoint * _prev;
    Waypoint * _next;

    LineObject *_lineObj;
    
};

#endif // WAYPOINT_H
