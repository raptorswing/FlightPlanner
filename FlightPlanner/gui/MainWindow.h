#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSharedPointer>
#include <QList>
#include <QPointer>

#include "ProblemViewAdapter.h"

#include "PlanningProblem.h"        //From PlanningCommon
#include "FlightPlanner.h"          //From PlanningCommon
#include "UserStudyChatHandler.h"   //From PlanningCommon
#include "WaysetDisplayManager.h"   //From PlanningCommon
#include "UserStudyEventLogger.h"   //From PlanningCommon

#include "MapGraphicsView.h"        //From MapGraphics
#include "MapGraphicsScene.h"       //From MapGraphics

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void openHiddenProblem(const QString& filePath);
    
private slots:
    void updateDisplayedFlight();
    void openProblem(const QString& filePath);
    void saveProblem(const QString& filePath);
    void resetAll();

    void handleFlightTaskAreaDrawn(const QSharedPointer<FlightTaskArea> area);

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
    void on_actionTest_Flight_triggered();

private:
    void initMap();
    void initPlanningProblem();
    void initPaletteConnections();
    void enableDisableFlightActions();
    void loadHiddenProblemAttributes();

    Ui::MainWindow *ui;

    QPointer<MapGraphicsView> _view;
    QPointer<MapGraphicsScene> _scene;

    QSharedPointer<PlanningProblem> _problem;
    QSharedPointer<PlanningProblem> _hiddenProblem;
    QPointer<FlightPlanner> _planner;
    QPointer<ProblemViewAdapter> _viewAdapter;

    QPointer<WaysetDisplayManager> _waysetManager;
};

#endif // MAINWINDOW_H
