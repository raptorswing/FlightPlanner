#include "StartPosMapObject.h"
#include <QKeyEvent>

StartPosMapObject::StartPosMapObject(QWeakPointer<PlanningProblem> prob) :
    CircleObject(10.0, true, QColor(0,255,0)),
    _prob(prob)
{
    QSharedPointer<PlanningProblem> strong = _prob.toStrongRef();
    connect(strong.data(),
            SIGNAL(startingPositionChanged(Position)),
            this,
            SLOT(handleStartPosChange(Position)));
}

void StartPosMapObject::setPos(const QPointF & pos)
{
    CircleObject::setPos(pos);

    QSharedPointer<PlanningProblem> strong = _prob.toStrongRef();
    if (strong->startingPosition().lonLat() != pos)
        strong->setStartingPosition(Position(pos,0.0));
}

void StartPosMapObject::keyReleaseEvent(QKeyEvent *event)
{
    event->ignore();
}

void StartPosMapObject::handleStartPosChange(const Position &pos)
{
    this->setPos(pos.lonLat());
}
