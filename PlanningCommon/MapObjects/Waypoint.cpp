#include "Waypoint.h"

#include <QMenu>
#include <QTimer>
#include <cmath>

#include "guts/Conversions.h"

#include "DubinsLineObject.h"
#include "LineObject.h"

#include "Dubins.h"

const qreal PI = 3.14159265359;

Waypoint::Waypoint(const QWeakPointer<PlanningProblem> &problem,
                   Waypoint *prev,
                   Waypoint *next,
                   WaypointLineMode lMode) :
    MapGraphicsObject(true),
    _displaySize(15.0),
    _problem(problem),
    _lineObj(0),
    _lMode(lMode)
{
    this->setPrev(prev);
    this->setNext(next);
    this->setZValue(6.0);
}

Waypoint::~Waypoint()
{
    if (this->prev() != 0)
        this->prev()->setNext(this->next());

    if (this->next() != 0)
        this->next()->setPrev(this->prev());

    if (_lineObj != 0)
        _lineObj->deleteLater();

    //A smelly hack so that WaypointManager can grab our next pointer if we're their first
    this->aboutToDelete(this->next());
}

Waypoint *Waypoint::prev() const
{
    return _prev;
}

Waypoint *Waypoint::next() const
{
    return _next;
}

int Waypoint::autoFixKinematicsIteration()
{
    int violations = 0;

    if (_problem.isNull())
        return 0;
    QSharedPointer<PlanningProblem> problem = _problem.toStrongRef();
    const qreal maxTurnAngle = problem->uavParameters().maxTurnAngle();

    if (!_prev.isNull() && !_next.isNull())
    {
        const qreal angleBefore = Waypoint::angleBetween(this->prev()->pos(),
                                                         this->pos());
        const qreal angleAfter = Waypoint::angleBetween(this->pos(),
                                                        this->next()->pos());

        const qreal error = Waypoint::angleAbsVal(angleBefore, angleAfter) - maxTurnAngle;
        if (error > 0.0)
        {
            //Perturb positions
            QPointF newPrevPos = Waypoint::perturbPosition(this->prev()->pos());
            QPointF newPos = Waypoint::perturbPosition(this->pos());
            QPointF newNextPos = Waypoint::perturbPosition(this->next()->pos());
            qreal currentError = error;
            while (currentError >= error)
            {
                newPrevPos = Waypoint::perturbPosition(this->prev()->pos());
                newPos = Waypoint::perturbPosition(this->pos());
                newNextPos = Waypoint::perturbPosition(this->next()->pos());

                const qreal angleBefore = Waypoint::angleBetween(newPrevPos,
                                                                 newPos);
                const qreal angleAfter = Waypoint::angleBetween(newPos,
                                                                newNextPos);
                currentError = Waypoint::angleAbsVal(angleBefore, angleAfter) - maxTurnAngle;
            }

            this->prev()->setPos(newPrevPos);
            this->setPos(newPos);
            this->next()->setPos(newNextPos);
        }
    }

    if (!_next.isNull())
        violations += _next->autoFixKinematicsIteration();

    /*
     * We have to check ourselves for violation again since someone farther along could have messed
     * us up.
    */
    if (!_prev.isNull() && !_next.isNull())
    {
        const qreal angleBefore = Waypoint::angleBetween(this->prev()->pos(),
                                                         this->pos());
        const qreal angleAfter = Waypoint::angleBetween(this->pos(),
                                                        this->next()->pos());
        const qreal error = Waypoint::angleAbsVal(angleBefore, angleAfter) - maxTurnAngle;
        if (error > 0.0)
            violations++;
    }

    return violations;
}

int Waypoint::autoFixDistancesIteration()
{
    const qreal threshold = 50.0;
    int violations = 0;

    const qreal lonPerMeter = Conversions::degreesLonPerMeter(this->pos().y());
    const qreal latPerMeter = Conversions::degreesLatPerMeter(this->pos().y());

    if (!_prev.isNull() && !_next.isNull())
    {
        Position prev(_prev->pos());
        Position next(_next->pos());
        Position me(this->pos());

        const qreal a = prev.flatDistanceEstimate(me);
        const qreal b = me.flatDistanceEstimate(next);

        if (qAbs<qreal>(a - b) > threshold)
        {
            Position moveToward = next;
            if (a > b)
                moveToward = prev;

            const qreal angle = me.angleTo(moveToward);
            QPointF newPos(me.longitude() + cos(angle)*5*lonPerMeter,
                           me.latitude() + sin(angle)*5*latPerMeter);
            this->setPos(newPos);
        }
    }

    if (!_next.isNull())
        violations += _next->autoFixDistancesIteration();

    if (!_prev.isNull() && !_next.isNull())
    {
        Position prev(_prev->pos());
        Position next(_next->pos());
        Position me(this->pos());

        const qreal a = prev.flatDistanceEstimate(me);
        const qreal b = me.flatDistanceEstimate(next);

        if (qAbs<qreal>(a - b) > threshold)
            violations++;
    }


    return violations;
}

//virtual from MapGraphicsObject
void Waypoint::setPos(const QPointF & pos)
{
    MapGraphicsObject::setPos(pos);
    this->updateLine();

    if (this->prev() != 0 && this->prev()->prev() != 0)
        this->prev()->prev()->updateLine();
}

//pure-virtual from MapGraphicsObject
QRectF Waypoint::boundingRect() const
{
    return QRectF(-1 * _displaySize,
                  -1 * _displaySize,
                  2 * _displaySize,
                  2 * _displaySize);
}

//pure-virtual from MapGraphicsObject
void Waypoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing, true);


    const QColor fillColor = QColor(255,0,0,50);
    QPolygonF triangle;
    triangle << QPointF(-0.5 * _displaySize, -0.5 * _displaySize);
    triangle << QPointF(0.5 * _displaySize, -0.5 * _displaySize);
    triangle << QPointF(0,0.6 * _displaySize);

    painter->setBrush(QBrush(fillColor));
    if (this->next() == 0)
    {
        painter->drawEllipse(QPointF(0,0),
                             _displaySize / 2.0,
                             _displaySize / 2.0);
    }
    else
    {
        painter->save();
        painter->rotate(this->angle().radians() * (180.0 / PI) - 90.0);
        painter->drawPolygon(triangle, Qt::OddEvenFill);
        painter->restore();
    }
}

UAVOrientation Waypoint::angle() const
{
    qreal toRet;
    if (_next.isNull() && _prev.isNull())
        toRet = 0.0;
    else if (_next.isNull())
        toRet = Waypoint::angleBetween(this->prev()->pos(), this->pos());
    else if (_prev.isNull())
        toRet = Waypoint::angleBetween(this->pos(), this->next()->pos());
    else
    {
        const UAVOrientation before(Waypoint::angleBetween(this->prev()->pos(), this->pos()));
        const UAVOrientation after(Waypoint::angleBetween(this->pos(), this->next()->pos()));
        const Position prevPos(this->prev()->pos());
        const Position pos(this->pos());
        const Position nextPos(this->next()->pos());

        const qreal prevDist = prevPos.flatDistanceEstimate(pos);
        const qreal nextDist = pos.flatDistanceEstimate(nextPos);
        const qreal totalDist = prevDist + nextDist;

        qreal prevWeight = 1.0 - prevDist / totalDist;
        qreal nextWeight = 1.0 - nextDist / totalDist;

        toRet = UAVOrientation::average(before, after, prevWeight, nextWeight).radians();
    }


    return UAVOrientation(toRet);
}

//public slot
void Waypoint::setPrev(Waypoint *nPrev)
{
    //Disconnect old signals
    if (!_prev.isNull())
    {
        disconnect(_prev.data(),
                   SIGNAL(posChanged()),
                   this,
                   SLOT(updateLine()));
    }

    //Set new prev
    _prev = nPrev;
    this->updateLine();

    //Connect signals
    if (!_prev.isNull())
    {
        connect(_prev.data(),
                SIGNAL(posChanged()),
                this,
                SLOT(updateLine()));
    }
}

//public slot
void Waypoint::setNext(Waypoint *nNext)
{
    //Disconnect signals from "old" next...
    if (!_next.isNull())
    {
        disconnect(_next.data(),
                   SIGNAL(posChanged()),
                   this,
                   SLOT(updateLine()));
    }

    //Set "new" next and update our line
    _next = nNext;
    this->updateLine();


    //Connect slots if appropriate
    if (nNext != 0)
    {
        connect(nNext,
                SIGNAL(posChanged()),
                this,
                SLOT(updateLine()));
    }

    if (!_prev.isNull())
        _prev->updateLine();
}

//public slot
void Waypoint::append(Waypoint *newWpt)
{
    if (newWpt == 0)
        return;

    if (this->next() == 0)
    {
        this->setNext(newWpt);
        newWpt->setPrev(this);

        //New waypoints should have the same flags (mouse interaction) as we do
        newWpt->setFlags(this->flags());
    }
    else
        this->next()->append(newWpt);
}

//public slot
void Waypoint::insertNewAfter()
{
    if (this->next() == 0)
        return;

    const QPointF avg = (this->pos() + this->next()->pos()) / 2.0;

    Waypoint * wpt = new Waypoint(_problem, 0, 0, _lMode);
    wpt->setPos(avg);

    //Announce the new waypoint before setting links
    //This way, signals needed to add lines, etc. will be caught
    this->newObjectGenerated(wpt);
    this->newWaypointGenerated(wpt);

    wpt->setPrev(this);
    wpt->setNext(this->next());

    this->next()->setPrev(wpt);
    this->setNext(wpt);
}

//public slot
void Waypoint::insertNewBefore()
{
    if (this->prev() == 0)
        return;

    const QPointF avg = (this->pos() + this->prev()->pos()) / 2.0;

    Waypoint * wpt = new Waypoint(_problem, 0, 0, _lMode);
    wpt->setPos(avg);

    //Announce the new waypoint before setting links
    //This way, signals needed to add lines, etc. will be caught
    this->newObjectGenerated(wpt);
    this->newWaypointGenerated(wpt);

    wpt->setNext(this);
    wpt->setPrev(this->prev());

    this->prev()->setNext(wpt);
    this->setPrev(wpt);
}

//public slot
void Waypoint::deleteAllFollowing()
{
    QPointer<Waypoint> next = this->next();
    while (!next.isNull())
    {
        next->deleteLater();
        next = next->next();
    }
    this->setNext(0);
}

//public slot
void Waypoint::deleteAllPrevious()
{
    QPointer<Waypoint> prev = this->prev();
    while (!prev.isNull())
    {
        prev->deleteLater();
        prev = prev->prev();
    }
    this->setPrev(0);
}

//protected
//virtual from MapGraphicsObject
void Waypoint::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    //Quick hack to check for mouse interactivity mode (or lack of it)
    if (!this->flags().testFlag(MapGraphicsObject::ObjectIsSelectable))
        return;

    event->accept();

    const bool hasNext = (this->next() != 0);
    const bool hasPrev = (this->prev() != 0);

    QMenu menu("Awesome menu");
    menu.addAction("Insert waypoint before", this, SLOT(insertNewBefore()))->setEnabled(hasPrev);
    menu.addAction("Insert waypoint after", this, SLOT(insertNewAfter()))->setEnabled(hasNext);
    menu.addSeparator();
    menu.addAction("Delete waypoint", this, SLOT(deleteLater()));
    menu.addAction("Delete all following", this, SLOT(deleteAllFollowing()))->setEnabled(hasNext);
    menu.addAction("Delete all previous", this, SLOT(deleteAllPrevious()))->setEnabled(hasPrev);

    menu.exec(event->screenPos());
}

//private slot
void Waypoint::updateLine()
{
    if (_problem.isNull())
        return;

    QSharedPointer<PlanningProblem> problem = _problem.toStrongRef();
    const qreal minTurnRadius = problem->uavParameters().minTurningRadius();

    if (this->next() == 0 && _lineObj != 0)
    {
        _lineObj->deleteLater();
        _lineObj = 0;
    }
    else if (this->next() != 0)
    {
        const qreal avgLat = (this->pos().y() + this->next()->pos().y()) / 2.0;
        const qreal lonPerMeter = Conversions::degreesLonPerMeter(avgLat);
        const qreal latPerMeter = Conversions::degreesLatPerMeter(avgLat);

        const QPointF startPos(0,0);
        const qreal startAngle = this->angle().radians();
        const QPointF endPos((this->next()->pos().x() - this->pos().x()) / lonPerMeter,
                             (this->next()->pos().y() - this->pos().y()) / latPerMeter);
        const qreal endAngle = this->next()->angle().radians();

        Dubins dubins(startPos, startAngle, endPos, endAngle, minTurnRadius);

        if (_lineObj == 0)
        {
            if (_lMode == DubinLineMode)
                _lineObj = new DubinsLineObject(Position(this->pos()), dubins);
            else
                _lineObj = new LineObject(this->pos(), this->next()->pos());

            this->newObjectGenerated(_lineObj);
        }
        else
        {
            if (_lMode == DubinLineMode)
            {
                DubinsLineObject * dubinLine = qobject_cast<DubinsLineObject *>(_lineObj);
                dubinLine->setDubins(Position(this->pos()), dubins);
            }
            else
            {
                LineObject * line = qobject_cast<LineObject *>(_lineObj);
                line->setEndPoints(this->pos(), this->next()->pos());
            }
        }
    }

    /*
     * Every time our line needs to be updated we'll also need to be redrawn probably.
    */
    this->redrawRequested();
}

//static private
qreal Waypoint::angleBetween(const QPointF &src, const QPointF &dst)
{
    const qreal avgLat = (src.y() + dst.y()) / 2.0;
    const qreal lonPerMeter = Conversions::degreesLonPerMeter(avgLat);
    const qreal latPerMeter = Conversions::degreesLatPerMeter(avgLat);

    const QPointF diffDeg = dst - src;
    const QPointF diffMeters(diffDeg.x() / lonPerMeter,
                             diffDeg.y() / latPerMeter);
    const qreal angle = atan2(diffMeters.y(), diffMeters.x());
    return angle;
}

//static private
qreal Waypoint::angleAbsVal(qreal angleA, qreal angleB)
{
    angleA = Waypoint::normalizeAngle(angleA);
    angleB = Waypoint::normalizeAngle(angleB);

    const qreal smallAngle1 = qMin<qreal>(angleA, angleB);
    const qreal smallAngle2 = smallAngle1 + (2.0 * PI);
    const qreal bigAngle = qMax<qreal>(angleA, angleB);

    const qreal radianDiff = qMin<qreal>(qAbs<qreal>(smallAngle1 - bigAngle),
                                         qAbs<qreal>(smallAngle2 - bigAngle));
    return radianDiff;
}

//static private
qreal Waypoint::normalizeAngle(qreal angle)
{
    while (angle > 2.0 * PI)
        angle -= 2.0 * PI;

    while (angle < 0.0)
        angle += 2.0 * PI;

    return angle;
}

//static private
QPointF Waypoint::perturbPosition(const QPointF &pos)
{
    const int maxMeters = 10.0;
    const qreal lonPerMeter = Conversions::degreesLonPerMeter(pos.y());
    const qreal latPerMeter = Conversions::degreesLatPerMeter(pos.y());

    const qreal lonPerturb = ((qrand() % (maxMeters + 1)) - maxMeters / 2) * lonPerMeter;
    const qreal latPerturb = ((qrand() % (maxMeters + 1)) - maxMeters / 2) * latPerMeter;

    return QPointF(pos.x() + lonPerturb,
                   pos.y() + latPerturb);
}
