#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSharedPointer>
#include <QList>
#include <QPointer>

#include "MapGraphicsView.h"
#include "MapGraphicsScene.h"
#include "PlanningProblem.h"
#include "FlightPlanner.h"
#include "ProblemViewAdapter.h"

#include "WaysetDisplayManager.h" //From PlanningCommon

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
    void updateDisplayedFlight();
    void openProblem(const QString& filePath);
    void saveProblem(const QString& filePath);
    void resetAll();

    //MainWindow actions
    void on_actionOpen_triggered();
    void on_actionSave_Planning_Problem_triggered();
    void on_actionNew_triggered();
    void on_actionClose_triggered();
    void on_actionExport_Solution_triggered();
    void on_actionExit_triggered();
    void on_actionUAV_Parameters_triggered();
    void on_actionImport_Solution_triggered();
    void on_actionPlan_Flight_triggered();
    void on_actionReset_Flight_triggered();
    void on_actionPlace_Start_Point_triggered();
    void on_actionPlace_Task_Area_triggered();

private:
    void initMap();
    void initPlanningProblem();
    void initPaletteConnections();

    Ui::MainWindow *ui;

    QPointer<MapGraphicsView> _view;
    QPointer<MapGraphicsScene> _scene;

    QSharedPointer<PlanningProblem> _problem;
    QPointer<FlightPlanner> _planner;
    QPointer<ProblemViewAdapter> _viewAdapter;

    QPointer<WaysetDisplayManager> _waysetManager;
};

#endif // MAINWINDOW_H
