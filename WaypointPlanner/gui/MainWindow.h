#ifndef WAYPOINTPLANNERMAINWINDOW_H
#define WAYPOINTPLANNERMAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <QSharedPointer>

#include "WaypointMapView.h"
#include "MapGraphicsScene.h"
#include "Waypoint.h"
#include "WaysetManager.h"
#include "PolygonObject.h"

#include "PlanningProblem.h"

namespace Ui {
class MainWindow;
}

class WaypointPlannerMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum MouseMode
    {
        SelectMode,
        CreateMode,
        CoverageHelperMode
    };

public:
    explicit WaypointPlannerMainWindow(QWidget *parent = 0);
    ~WaypointPlannerMainWindow();

signals:
    void planningProblemChanged(const QSharedPointer<PlanningProblem>& problem);

public slots:
    void setMouseMode(MouseMode mode);
    void setPlanningProblem(const QSharedPointer<PlanningProblem>& problem);
    
private slots:
    void doInitialMapCentering();
    void handleMapClick(QPoint pos);
    void on_actionExit_triggered();
    void on_actionSelect_Mode_triggered();
    void on_actionCreate_Mode_triggered();
    void on_actionAuto_Fix_triggered();
    void on_actionCoverage_Helper_triggered();

private:
    void restoreGeometry();
    void storeGeometry();
    void initProblem();
    void initMap();
    Ui::MainWindow *ui;

    WaypointMapView * _view;
    MapGraphicsScene * _scene;

    MouseMode _mouseMode;
    QSharedPointer<PlanningProblem> _problem;
    WaysetManager * _waysetManager;

    QPointer<PolygonObject> _coveragePolygon;
};

#endif // WAYPOINTPLANNERMAINWINDOW_H
