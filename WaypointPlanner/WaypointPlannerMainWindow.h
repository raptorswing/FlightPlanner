#ifndef WAYPOINTPLANNERMAINWINDOW_H
#define WAYPOINTPLANNERMAINWINDOW_H

#include <QMainWindow>

#include "MapGraphicsView.h"
#include "MapGraphicsScene.h"

namespace Ui {
class WaypointPlannerMainWindow;
}

class WaypointPlannerMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit WaypointPlannerMainWindow(QWidget *parent = 0);
    ~WaypointPlannerMainWindow();
    
private slots:
    void on_actionExit_triggered();

private:
    void restoreGeometry();
    void storeGeometry();
    void initMap();
    Ui::WaypointPlannerMainWindow *ui;

    MapGraphicsView * _view;
    MapGraphicsScene * _scene;
};

#endif // WAYPOINTPLANNERMAINWINDOW_H
