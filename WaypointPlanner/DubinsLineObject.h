#ifndef DUBINSLINEOBJECT_H
#define DUBINSLINEOBJECT_H

#include "LineObject.h"
#include "Dubins.h"
#include "Position.h"

#include <QPointer>

class DubinsLineObject : public LineObject
{
    Q_OBJECT
public:
    DubinsLineObject(const Position& rootPos,
                     const Dubins& dubins,
                     qreal dubinsStart,
                     MapGraphicsObject *parent = 0);
    virtual ~DubinsLineObject();

    //pure-virtual from LineObject
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    
signals:
    
public slots:
    void setDubins(Position rootPos,
                   Dubins dubins,
                   qreal dubinsStart);

private slots:
    void updateDubins();

private:
    Position _rootPos;
    Dubins _dubins;
    qreal _dubinsStart;

    QPointer<DubinsLineObject> _next;
    
};

#endif // DUBINSLINEOBJECT_H
