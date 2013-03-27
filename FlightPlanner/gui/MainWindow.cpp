#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QtDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>

#include "MapGraphicsView.h"
#include "MapGraphicsScene.h"
#include "tileSources/OSMTileSource.h"

#include "HierarchicalPlanner/HierarchicalPlanner.h"

#include "UAVParametersWidget.h"

#include "gui/CommonFileHandling.h"
#include "gui/CommonWindowHandling.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _view(0), _scene(0),
    _planner(0), _viewAdapter(0)
{
    ui->setupUi(this);

    //Restore window geometry
    CommonWindowHandling::restoreGeometry(this);

    this->initMap();
    this->initPlanningProblem();
    this->initPaletteConnections();
    this->initPlanningControlConnections();

    qsrand(QDateTime::currentMSecsSinceEpoch());
}

MainWindow::~MainWindow()
{
    //Store window geometry
    CommonWindowHandling::storeGeometry(this);
    delete ui;
}

//private slot
void MainWindow::on_actionOpen_triggered()
{
    const QString filePath = QFileDialog::getOpenFileName(this,
                                                          "Select file to load");
    if (filePath.isEmpty())
        return;

    QFile fp(filePath);
    if (!fp.exists())
        return;
    else if (!fp.open(QFile::ReadOnly))
    {
        QMessageBox::warning(this, "Error", "Failed to open file for reading");
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
void MainWindow::on_actionSave_Planning_Problem_triggered()
{
    if (_problem.isNull())
        return;

    const QString filePath = QFileDialog::getSaveFileName(this,
                                                          "Select save file");
    if (filePath.isEmpty())
        return;

    QFile fp(filePath);
    if (!fp.open(QFile::WriteOnly))
    {
        QMessageBox::warning(this,
                             "Error",
                             "Failed to open save file for writing.");
        return;
    }

    QDataStream stream(&fp);
    _problem->serialize(stream);
}

//private slot
void MainWindow::on_actionSave_As_triggered()
{

}

//private slot
void MainWindow::on_actionNew_triggered()
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
void MainWindow::on_actionClose_triggered()
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
void MainWindow::on_actionExport_Solution_triggered()
{
    const Wayset& solution = _planner->bestFlightSoFar();
    CommonFileHandling::doExport(solution, QString(), this);
}

//private slot
void MainWindow::on_actionUndo_triggered()
{

}

//private slot
void MainWindow::on_actionRedo_triggered()
{

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
void MainWindow::on_actionSensor_Parameters_triggered()
{

}

//private slot
void MainWindow::handleAddStartPointRequested()
{
    _problem->setStartingPosition(Position(_view->center(), 0.0));
    _problem->setStartingOrientation(0.0);
}

//private slot
void MainWindow::handleAddTaskAreaRequested()
{
    _problem->addTaskArea(_view->center());
}

//private slot
void MainWindow::handlePlanningStartRequested()
{
    if (!_problem->startingOrientationDefined() || !_problem->startingPositionDefined())
    {
        QMessageBox::information(this,
                                 "Can't Start Planning",
                                 "Planning cannot begin until a start point is defined");
        return;
    }
    _planner->startPlanning();
}

//private slot
void MainWindow::handlePlanningPauseRequested()
{
    _planner->pausePlanning();
}

//private slot
void MainWindow::handlePlanningClearRequested()
{
    _planner->pausePlanning();
    _planner->resetPlanning();
}

//private slot
void MainWindow::handlePlannerProgressChanged(qreal fitness, quint32 iterations)
{
    this->ui->planningControlWidget->setPlanningProgress(iterations, fitness);
}

//private slot
void MainWindow::handlePlannerStatusChanged(FlightPlanner::PlanningStatus status)
{
    this->ui->planningControlWidget->setPlanningState(status);

    if (status == FlightPlanner::Paused || status == FlightPlanner::Stopped)
        this->updateDisplayedFlight();
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
    QSharedPointer<MapTileSource> osm(new OSMTileSource());
    QSharedPointer<MapTileSource> mqSat(new OSMTileSource(OSMTileSource::MapQuestAerialTiles));
    composite->addSourceBottom(osm,0.75);
    composite->addSourceBottom(mqSat);
    _view->setTileSource(composite);

    //Provide our "map layers" dock widget with the composite tile source to be configured
    this->ui->mapLayersWidget->setComposite(composite);

    //Zoom into BYU campus by default
    QPointF place(-111.649253,40.249707);
    _view->setZoomLevel(15);
    _view->centerOn(place);
}

//private
void MainWindow::initPlanningProblem()
{
    _problem = QSharedPointer<PlanningProblem>(new PlanningProblem());
    //_planner = new GreedyFlightPlanner(_problem, this);
    _planner = new HierarchicalPlanner(_problem, this);
    connect(_planner,
            SIGNAL(plannerStatusChanged(FlightPlanner::PlanningStatus)),
            this,
            SLOT(handlePlannerStatusChanged(FlightPlanner::PlanningStatus)));
    connect(_planner,
            SIGNAL(plannerProgressChanged(qreal,quint32)),
            this,
            SLOT(handlePlannerProgressChanged(qreal,quint32)));
    connect(_problem.data(),
            SIGNAL(planningProblemChanged()),
            _planner,
            SLOT(resetPlanning()));

    _viewAdapter = new ProblemViewAdapter(_problem,
                                          _scene,
                                          this);

    _waysetManager = new WaysetDisplayManager(_scene, _problem, Waypoint::StraightLineMode, this);
    _waysetManager->enableMouseInteraction(true);
}

//private
void MainWindow::initPaletteConnections()
{
    if (!this->ui->paletteWidget)
    {
        qWarning() << "Palette widget not initialized";
        return;
    }

    connect(this->ui->paletteWidget,
            SIGNAL(addStartPointRequested()),
            this,
            SLOT(handleAddStartPointRequested()));

    connect(this->ui->paletteWidget,
            SIGNAL(addTaskAreaRequested()),
            this,
            SLOT(handleAddTaskAreaRequested()));
}

//private
void MainWindow::initPlanningControlConnections()
{
    if (!this->ui->planningControlWidget)
    {
        qWarning() << "Planning control widget not initialized";
        return;
    }

    connect(this->ui->planningControlWidget,
            SIGNAL(planningStartRequested()),
            this,
            SLOT(handlePlanningStartRequested()));

    connect(this->ui->planningControlWidget,
            SIGNAL(planningPauseRequested()),
            this,
            SLOT(handlePlanningPauseRequested()));

    connect(this->ui->planningControlWidget,
            SIGNAL(planningClearRequested()),
            this,
            SLOT(handlePlanningClearRequested()));
}

void MainWindow::updateDisplayedFlight()
{
    _waysetManager->setPlanningProblem(_problem);
    _waysetManager->setWayset(_planner->bestFlightSoFar(), Waypoint::StraightLineMode);
}
