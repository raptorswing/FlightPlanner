#ifndef DUBINSLINEOBJECT_H
#define DUBINSLINEOBJECT_H

#include "MapGraphicsObject.h"
#include "Dubins.h"
#include "Position.h"

#include <QPointer>
#include <QVector>

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT DubinsLineObject : public MapGraphicsObject
{
    Q_OBJECT
public:
    DubinsLineObject(const Position& rootPos,
                     const Dubins& dubins,
                     qreal thickness = 2.0,
                     MapGraphicsObject *parent = 0);
    virtual ~DubinsLineObject();

    //pure-virtual from MapGraphicsObject
    QRectF boundingRect() const;

    //pure-virtual from MapGraphicsObject
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    qreal thickness() const;
    void setThickness(qreal nThick);
    
signals:
    
public slots:
    void setDubins(Position rootPos,
                   Dubins dubins);

private slots:
    void updateDubins();

private:
    Position _rootPos;
    Dubins _dubins;
    qreal _thickness;

    QRectF _boundingRect;
    QVector<QPointF> _drawOffsets;
    
};

#endif // DUBINSLINEOBJECT_H
