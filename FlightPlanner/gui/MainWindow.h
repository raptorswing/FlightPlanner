#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSharedPointer>
#include <QList>

#include "MapGraphicsView.h"
#include "MapGraphicsScene.h"
#include "PlanningProblem.h"
#include "FlightPlanner.h"
#include "ProblemViewAdapter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    //MainWindow actions
    void on_actionOpen_triggered();
    void on_actionSave_Planning_Problem_triggered();
    void on_actionSave_As_triggered();
    void on_actionNew_triggered();
    void on_actionClose_triggered();
    void on_actionExport_Solution_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_actionExit_triggered();
    void on_actionUAV_Parameters_triggered();
    void on_actionSensor_Parameters_triggered();

    //Palette Widget actions
    void handleAddStartPointRequested();
    void handleAddTaskAreaRequested();

    //Planning control widget actions
    void handlePlanningStartRequested();
    void handlePlanningPauseRequested();
    void handlePlanningClearRequested();

    //Planner events
    void handlePlannerProgressChanged(qreal fitness, quint32 iterations);
    void handlePlannerStatusChanged(FlightPlanner::PlanningStatus status);

private:
    inline void initMap();
    inline void initPlanningProblem();
    inline void initPaletteConnections();
    inline void initPlanningControlConnections();
    void updateDisplayedFlight();

    Ui::MainWindow *ui;

    MapGraphicsView * _view;
    MapGraphicsScene * _scene;

    QSharedPointer<PlanningProblem> _problem;
    FlightPlanner * _planner;
    ProblemViewAdapter * _viewAdapter;

    QList<MapGraphicsObject *> _displayedPathObjects;
};

#endif // MAINWINDOW_H
