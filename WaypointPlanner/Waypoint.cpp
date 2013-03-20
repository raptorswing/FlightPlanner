#include "Waypoint.h"

#include <QMenu>
#include <QTimer>

Waypoint::Waypoint(Waypoint *prev, Waypoint *next) :
    CircleObject(6, true, Qt::yellow), _lineObj(0)
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

//virtual from MapGraphicsObject
void Waypoint::setPos(const QPointF & pos)
{
    CircleObject::setPos(pos);
    this->updateLine();
}

//public slot
void Waypoint::setPrev(Waypoint *nPrev)
{
    _prev = nPrev;
}

//public slot
void Waypoint::setNext(Waypoint *nNext)
{
    _next = nNext;
    this->updateLine();

    if (nNext != 0)
    {
        connect(nNext,
                SIGNAL(posChanged()),
                this,
                SLOT(updateLine()));
    }
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

    Waypoint * wpt = new Waypoint();
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

    Waypoint * wpt = new Waypoint();
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

//protected
//virtual from MapGraphicsObject
void Waypoint::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    event->accept();

    const bool hasNext = (this->next() != 0);
    const bool hasPrev = (this->prev() != 0);

    QMenu menu("Awesome menu");
    menu.addAction("Insert waypoint before", this, SLOT(insertNewBefore()))->setEnabled(hasPrev);
    menu.addAction("Insert waypoint after", this, SLOT(insertNewAfter()))->setEnabled(hasNext);
    menu.addSeparator();
    menu.addAction("Delete waypoint", this, SLOT(deleteLater()));

    menu.exec(event->screenPos());
}

//private slot
void Waypoint::updateLine()
{
    if (this->next() == 0 && _lineObj != 0)
    {
        _lineObj->deleteLater();
        _lineObj = 0;
    }
    else if (this->next() != 0)
    {
        if (_lineObj == 0)
        {
            _lineObj = new LineObject(this->pos(),
                                      this->next()->pos());

            this->newObjectGenerated(_lineObj);
        }
        else
        {
            _lineObj->setEndPoints(this->pos(),
                                   this->next()->pos());
        }
    }
}
