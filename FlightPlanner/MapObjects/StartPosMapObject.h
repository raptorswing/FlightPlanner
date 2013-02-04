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

protected:
    virtual void keyReleaseEvent(QKeyEvent *event);
    
signals:
    
public slots:
    void handleStartPosChange(const Position& pos);

private:
    QWeakPointer<PlanningProblem> _prob;
    
};

#endif // STARTPOSMAPOBJECT_H
