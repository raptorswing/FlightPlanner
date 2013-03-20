#ifndef WAYPOINTPLANNERMAINWINDOW_H
#define WAYPOINTPLANNERMAINWINDOW_H

#include <QMainWindow>
#include <QPointer>

#include "WaypointMapView.h"
#include "MapGraphicsScene.h"
#include "Waypoint.h"
#include "WaysetManager.h"

namespace Ui {
class WaypointPlannerMainWindow;
}

class WaypointPlannerMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum MouseMode
    {
        SelectMode,
        CreateMode
    };

public:
    explicit WaypointPlannerMainWindow(QWidget *parent = 0);
    ~WaypointPlannerMainWindow();

public slots:
    void setMouseMode(MouseMode mode);
    
private slots:
    void doInitialMapCentering();
    void handleMapClick(QPoint pos);
    void on_actionExit_triggered();
    void on_actionSelect_Mode_triggered();
    void on_actionCreate_Mode_triggered();
    void on_actionAuto_Fix_triggered();

private:
    void restoreGeometry();
    void storeGeometry();
    void initMap();
    Ui::WaypointPlannerMainWindow *ui;

    WaypointMapView * _view;
    MapGraphicsScene * _scene;

    MouseMode _mouseMode;
    WaysetManager * _waysetManager;
};

#endif // WAYPOINTPLANNERMAINWINDOW_H
