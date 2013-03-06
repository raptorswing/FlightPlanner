#ifndef STARTPOSMAPOBJECT_H
#define STARTPOSMAPOBJECT_H

#include <QWeakPointer>

#include "CircleObject.h"
#include "Position.h"
#include "PlanningProblem.h"

class StartPosMapObject : public CircleObject
{
    Q_OBJECT
public:
    explicit StartPosMapObject(QWeakPointer<PlanningProblem> prob);

    virtual void setPos(const QPointF &);

    const UAVOrientation& orientation() const;
    void setOrientation(const UAVOrientation& orient);

    //virtual from CircleObject
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    
signals:
    
public slots:
    void handleStartPosChange(const Position& pos);

private:
    QWeakPointer<PlanningProblem> _prob;
    UAVOrientation _orientation;

    UAVOrientation _angleDragStartOrientation;
    int _angleDragDist;
    
};

#endif // STARTPOSMAPOBJECT_H
