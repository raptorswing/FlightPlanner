#ifndef FANCYMAPGRAPHICSVIEW_H
#define FANCYMAPGRAPHICSVIEW_H

#include "MapGraphicsView.h"
#include "FlightTaskArea.h"
#include "CircleObject.h"

#include <QPolygonF>
#include <QSharedPointer>
#include <QSet>

class FancyMapGraphicsView : public MapGraphicsView
{
    Q_OBJECT
public:
    explicit FancyMapGraphicsView(MapGraphicsScene * scene=0, QWidget * parent = 0);

protected:
    virtual void handleChildMouseMove(QMouseEvent * event);
    virtual void handleChildMousePress(QMouseEvent * event);
    virtual void handleChildMouseRelease(QMouseEvent * event);
    
signals:
    void flightTaskAreaDrawn(const QSharedPointer<FlightTaskArea>& area);
    
public slots:

private:
    bool _rightPressed;
    QPolygonF _drawnPolygon;

    QSet<CircleObject *> _tempDrawMarkers;
    
};

#endif // FANCYMAPGRAPHICSVIEW_H
