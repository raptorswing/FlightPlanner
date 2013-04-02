#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QtDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>

#include "MapGraphicsView.h"
#include "MapGraphicsScene.h"
#include "tileSources/CompositeTileSource.h"
#include "tileSources/OSMTileSource.h"

#include "HierarchicalPlanner/HierarchicalPlanner.h"

#include "UAVParametersWidget.h"

#include "gui/CommonFileHandling.h"
#include "gui/CommonWindowHandling.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Restore window geometry
    CommonWindowHandling::restoreGeometry(this);

    this->initMap();
    this->initPlanningProblem();

    qsrand(QDateTime::currentMSecsSinceEpoch());
}

MainWindow::~MainWindow()
{
    //Store window geometry
    CommonWindowHandling::storeGeometry(this);

    delete ui;
}

//private slot
void MainWindow::updateDisplayedFlight()
{
    if (_problem.isNull())
        qWarning() << "Can't update displayed flight - problem is null";
    else if (_planner.isNull())
        qWarning() << "Can't update displayed flight - planner is null";
    else if (_waysetManager.isNull())
        qWarning() << "Can't update dispalyed flight - wayset manager is null";
    else
    {
        _waysetManager->setPlanningProblem(_problem);
        _waysetManager->setWayset(_planner->bestFlightSoFar());
    }
}

//private slot
void MainWindow::openProblem(const QString &filePath)
{
    QFile fp(filePath);
    if (!fp.exists())
        return;
    else if (!fp.open(QFile::ReadOnly))
    {
        QMessageBox::warning(this, "Error - cannot open file",
                             "Failed to open file " + filePath + " for reading");
        return;
    }

    QDataStream stream(&fp);
    _problem = QSharedPointer<PlanningProblem>(new PlanningProblem(stream));
    _planner->setProblem(_problem);
    _viewAdapter->setModel(_problem);

    connect(_problem.data(),
            SIGNAL(planningProblemChanged()),
            _planner,
            SLOT(resetPlanning()));
}

//private slot
void MainWindow::saveProblem(const QString &filePath)
{
    QFile fp(filePath);
    if (!fp.open(QFile::WriteOnly | QFile::Truncate))
    {
        QMessageBox::warning(this,
                             "Error",
                             "Failed to open save file " + filePath + " for writing.");
        return;
    }

    QDataStream stream(&fp);
    _problem->serialize(stream);
}

//private slot
void MainWindow::resetAll()
{
    _problem = QSharedPointer<PlanningProblem>(new PlanningProblem());
    _planner->setProblem(_problem);
    _viewAdapter->setModel(_problem);

    connect(_problem.data(),
            SIGNAL(planningProblemChanged()),
            _planner,
            SLOT(resetPlanning()));
}

//private slot
void MainWindow::on_actionOpen_triggered()
{
    const QString filePath = QFileDialog::getOpenFileName(this,
                                                          "Select file to load");
    if (filePath.isEmpty())
        return;

    this->openProblem(filePath);
}

//private slot
void MainWindow::on_actionSave_Planning_Problem_triggered()
{
    if (_problem.isNull())
        return;

    const QString filePath = QFileDialog::getSaveFileName(this,
                                                          "Select save file");
    if (filePath.isEmpty())
        return;

    this->saveProblem(filePath);
}

//private slot
void MainWindow::on_actionNew_triggered()
{
    this->resetAll();
}

//private slot
void MainWindow::on_actionClose_triggered()
{
    this->resetAll();
}

//private slot
void MainWindow::on_actionExport_Solution_triggered()
{
    const Wayset& solution = _planner->bestFlightSoFar();
    UAVParameters params = _problem->uavParameters();
    params.setMinTurningRadius(params.minTurningRadius() * 0.99);
    Wayset toExport = solution.resample(3.0, params);
    CommonFileHandling::doExport(toExport, QString(), this);
}

//private slot
void MainWindow::on_actionExit_triggered()
{
    this->close();
}

//private slot
void MainWindow::on_actionUAV_Parameters_triggered()
{
    UAVParametersWidget * configWidget = new UAVParametersWidget(_problem);
    configWidget->setAttribute(Qt::WA_DeleteOnClose);
    connect(this,
            SIGNAL(destroyed()),
            configWidget,
            SLOT(deleteLater()));
    configWidget->show();
}

//private slot
void MainWindow::on_actionImport_Solution_triggered()
{
    bool ok;
    Wayset results = CommonFileHandling::doImport(ok, QString(), this);

    if (!ok)
        return;

    _planner->setBestFlightSoFar(results);
    this->updateDisplayedFlight();
}

//private slot
void MainWindow::on_actionPlan_Flight_triggered()
{
    if (_problem.isNull())
        qWarning() << "Can't start planning - problem is null";
    else if (_planner.isNull())
        qWarning() << "Can't start planning - planner is null";
    else if (!_problem->startingOrientationDefined() || !_problem->startingPositionDefined())
    {
        QMessageBox::information(this, "Can't start planning",
                                 "Planning cannot begin until a start point is defined");
    }
    else
        _planner->startPlanning();
}

//private slot
void MainWindow::on_actionReset_Flight_triggered()
{
    if (_planner.isNull())
        qWarning() << "Can't reset planning - planner is null";
    else
        _planner->resetPlanning();
}

//private slot
void MainWindow::on_actionPlace_Start_Point_triggered()
{
    if (_view.isNull())
        qWarning() << "Can't place start position - view is null";
    else
    {
        const Position startPos(_view->center());

        _problem->setStartingPosition(startPos);
        _problem->setStartingOrientation(0.0);
    }
}

//private slot
void MainWindow::on_actionPlace_Task_Area_triggered()
{
    if (_view.isNull())
        qWarning() << "Can't place task area - view is null";
    else
        _problem->addTaskArea(_view->center());
}


//private
void MainWindow::initMap()
{
    //Setup MapGraphicsScene and View
    _scene = new MapGraphicsScene(this);
    _view = new MapGraphicsView(_scene,this);
    this->setCentralWidget(_view);

    //Setup Map tile sources
    QSharedPointer<CompositeTileSource> composite(new CompositeTileSource());
    QSharedPointer<MapTileSource> osm(new OSMTileSource(OSMTileSource::MapQuestOSMTiles));
    composite->addSourceBottom(osm);
    _view->setTileSource(composite);

    //Zoom into BYU campus by default
    QPointF place(-111.649253,40.249707);
    _view->setZoomLevel(15);
    _view->centerOn(place);
}

//private
void MainWindow::initPlanningProblem()
{
    _problem = QSharedPointer<PlanningProblem>(new PlanningProblem());

    _planner = new HierarchicalPlanner(_problem, this);

    //When the problem changes the planner should reset
    connect(_problem.data(),
            SIGNAL(planningProblemChanged()),
            _planner,
            SLOT(resetPlanning()));

    //When the planner's state changes we should update our display
    connect(_planner.data(),
            SIGNAL(plannerStatusChanged(FlightPlanner::PlanningStatus)),
            this,
            SLOT(updateDisplayedFlight()));

    _viewAdapter = new ProblemViewAdapter(_problem,
                                          _scene,
                                          this);

    _waysetManager = new WaysetDisplayManager(_scene, _problem, Waypoint::DubinLineMode, this);
    _waysetManager->enableMouseInteraction(false);
}
