#ifndef FLIGHTTASKAREAMAPOBJECT_H
#define FLIGHTTASKAREAMAPOBJECT_H

#include "PolygonObject.h"
#include "FlightTaskArea.h"
#include "PlanningProblem.h"

#include <QWeakPointer>

class FlightTaskAreaMapObject : public PolygonObject
{
    Q_OBJECT
public:
    //We take the planning problem as input so that we can delete the area from within it when needed
    explicit FlightTaskAreaMapObject(QWeakPointer<PlanningProblem> problem,
                                     QWeakPointer<FlightTaskArea> flightTaskArea,
                                     QColor fillColor = QColor(200,200,200,200));

    QWeakPointer<FlightTaskArea> flightTaskArea() const;

    //virtual from PolygonObject
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
    
signals:
    
public slots:

private slots:
    void spawnConfigurationWidget();
    void handleFlightAreaGeoPolyChanged(const QPolygonF& geoPoly);
    void handleUserDeleteRequest();

private:
    QWeakPointer<PlanningProblem> _prob;
    QWeakPointer<FlightTaskArea> _area;
    
};

#endif // FLIGHTTASKAREAMAPOBJECT_H
