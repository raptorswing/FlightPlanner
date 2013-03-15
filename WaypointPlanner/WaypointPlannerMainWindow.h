#ifndef WAYPOINTPLANNERMAINWINDOW_H
#define WAYPOINTPLANNERMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class WaypointPlannerMainWindow;
}

class WaypointPlannerMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit WaypointPlannerMainWindow(QWidget *parent = 0);
    ~WaypointPlannerMainWindow();
    
private:
    Ui::WaypointPlannerMainWindow *ui;
};

#endif // WAYPOINTPLANNERMAINWINDOW_H
