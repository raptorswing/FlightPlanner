#ifndef WAYPOINTMAPVIEW_H
#define WAYPOINTMAPVIEW_H

#include "MapGraphicsView.h"

#include <QPoint>

class WaypointMapView : public MapGraphicsView
{
    Q_OBJECT
public:
    explicit WaypointMapView(MapGraphicsScene * scene=0, QWidget * parent = 0);
    
signals:
    void mapClicked(QPoint pos);
    
public slots:

protected slots:
    //virtual from MapGraphicsView
    virtual void handleChildMousePress(QMouseEvent *event);

    //virtual from MapGraphicsView
    virtual void handleChildMouseRelease(QMouseEvent *event);

private:
    bool _clickActive;
    QPoint _clickPos;
    
};

#endif // WAYPOINTMAPVIEW_H
