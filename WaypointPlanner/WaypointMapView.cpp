#include "WaypointMapView.h"

#include <QtDebug>

WaypointMapView::WaypointMapView(MapGraphicsScene *scene, QWidget *parent) :
    MapGraphicsView(scene, parent)
{
}


//protected slot
//virtual from MapGraphicsView
void WaypointMapView::handleChildMousePress(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    /*
     *Not accepted since we can't tell whether this will be a click or a drag until
     *we see the release event. Drags need to be handled by parent to support slippymap.
    */
    event->setAccepted(false);

    _clickActive = true;
    _clickPos = event->pos();
}

//protected slot
//virtual from MapGraphicsView
void WaypointMapView::handleChildMouseRelease(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;
    /*
     *Not accepted since we can't tell whether this will be a click or a drag until
     *we see the release event. Drags need to be handled by parent to support slippymap.
    */
    event->setAccepted(false);

    if (!_clickActive)
        return;
    _clickActive = false;

    const QPoint pos = event->pos();
    if ((pos - _clickPos).manhattanLength() < 7)
        this->mapClicked(pos);
}
