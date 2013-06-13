#include "FancyMapGraphicsView.h"

#include <QMouseEvent>
#include <QtDebug>

FancyMapGraphicsView::FancyMapGraphicsView(MapGraphicsScene *scene,
                                           QWidget *parent) :
    MapGraphicsView(scene, parent)
{
    _rightPressed = false;
}

//virtual protected
void FancyMapGraphicsView::handleChildMouseMove(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::RightButton))
        return;
    else if (!_rightPressed)
        return;

    const QPoint mousePoint = event->pos();
    const QPointF lonLat = this->mapToScene(mousePoint);
    const Position mousePos(lonLat);


    if (_drawnPolygon.isEmpty()
            || Position(_drawnPolygon.last()).flatDistanceEstimate(mousePos) > 50.0)
    {
        _drawnPolygon.append(lonLat);

        CircleObject * tempDrawMarker = new CircleObject(2.0, true, Qt::red);
        tempDrawMarker->setPos(lonLat);
        _tempDrawMarkers.insert(tempDrawMarker);
        this->scene()->addObject(tempDrawMarker);
    }

}

//virtual protected
void FancyMapGraphicsView::handleChildMousePress(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        event->setAccepted(true);
        _rightPressed = true;
    }
}

//virtual protected
void FancyMapGraphicsView::handleChildMouseRelease(QMouseEvent *event)
{
    if (event->button() != Qt::RightButton)
        return;

    event->setAccepted(true);
    _rightPressed = false;

    QPolygonF poly = _drawnPolygon;
    _drawnPolygon.clear();

    foreach(CircleObject * tempMarker, _tempDrawMarkers)
    {
        /*
         * We don't need to remove before we delete
        */
        //this->scene()->removeObject(tempMarker);
        delete tempMarker;
    }
    _tempDrawMarkers.clear();

    if (poly.size() < 3)
        return;

    QSharedPointer<FlightTaskArea> area(new FlightTaskArea(poly));

    this->flightTaskAreaDrawn(area);
}
