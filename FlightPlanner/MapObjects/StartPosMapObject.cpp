#include "StartPosMapObject.h"

#include <QtDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QVector2D>

#include <cmath>

StartPosMapObject::StartPosMapObject(QWeakPointer<PlanningProblem> prob) :
    CircleObject(10.0, true, QColor(0,255,0)),
    _prob(prob),
    _angleDragDist(-1)
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

const UAVOrientation &StartPosMapObject::orientation() const
{
    return _orientation;
}

void StartPosMapObject::setOrientation(const UAVOrientation &orient)
{
    _orientation = orient;
    this->redrawRequested();
}

//virtual from CircleObject
void StartPosMapObject::paint(QPainter *painter,
                              const QStyleOptionGraphicsItem *option,
                              QWidget *widget)
{
    CircleObject::paint(painter, option, widget);

    QVector2D vec(::cos(_orientation.radians()),
                  ::sin(_orientation.radians()));
    vec.normalize();
    vec *= this->radius();

    painter->drawLine(0,0, vec.x(), vec.y());
}

//protected virtual from CircleObject
void StartPosMapObject::keyReleaseEvent(QKeyEvent *event)
{
    event->ignore();
}

//protected virtual from MapGraphicsObject
void StartPosMapObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::RightButton)
        CircleObject::mousePressEvent(event);
    else
        event->accept();

}

//protected virtual from MapGraphicsObject
void StartPosMapObject::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!event->buttons().testFlag(Qt::RightButton))
        CircleObject::mouseMoveEvent(event);
    else
    {
        QSharedPointer<PlanningProblem> prob = _prob.toStrongRef();
        if (prob.isNull())
            return;

        if (_angleDragDist < 0)
        {
            _angleDragDist = event->pos().manhattanLength();
            _angleDragStartOrientation = _orientation;
        }

        int delta = event->pos().manhattanLength() - _angleDragDist;

        prob->setStartingOrientation(_angleDragStartOrientation.radians() + delta*3.14159265/180.0);
    }
}

//protected virtual from MapGraphicsObject
void StartPosMapObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::RightButton)
        CircleObject::mouseReleaseEvent(event);
    else
    {
        _angleDragDist = -1;
    }
}

//public slot
void StartPosMapObject::handleStartPosChange(const Position &pos)
{
    this->setPos(pos.lonLat());
}
