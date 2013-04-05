#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "tileSources/OSMTileSource.h"

#include <QSettings>
#include <QDesktopWidget>
#include <QtDebug>
#include <QTimer>
#include <QFileDialog>

#include "gui/CommonFileHandling.h"
#include "gui/CommonWindowHandling.h"

#include "FlightTaskArea.h"
#include "FlightTasks/CoverageTask.h"
#include "HierarchicalPlanner/SubFlightPlanner/SubFlightPlanner.h"
#include "SimulatedFlier.h"

WaypointPlannerMainWindow::WaypointPlannerMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->ui->setupUi(this);
    CommonWindowHandling::restoreGeometry(this);

    this->initProblem();
    this->initMap();
    this->enableDisableFlightActions();

    //A hack to make initial map centering work right.
    QTimer::singleShot(1, this, SLOT(doInitialMapCentering()));
}

WaypointPlannerMainWindow::~WaypointPlannerMainWindow()
{
    CommonWindowHandling::storeGeometry(this);
    delete ui;
}

//public slot
void WaypointPlannerMainWindow::setMouseMode(WaypointPlannerMainWindow::MouseMode mode)
{
    _mouseMode = mode;

    bool select = false, create = false, coverage = false;

    if (!_coveragePolygon.isNull())
        this->finishCoverageHelper();

    switch(_mouseMode)
    {
    case SelectMode:
        select = true;
        break;

    case CreateMode:
        create = true;
        break;

    case CoverageHelperMode:
        coverage = true;
        break;
    }

    this->ui->actionSelect_Mode->setChecked(select);
    this->ui->actionCreate_Mode->setChecked(create);
    this->ui->actionCoverage_Helper->setChecked(coverage);

    //Waypoints can only be interacted with in Select/Edit mode
    _waysetManager->enableMouseInteraction(mode == SelectMode);
}

//public slot
void WaypointPlannerMainWindow::setPlanningProblem(const QSharedPointer<PlanningProblem> &problem)
{
    _problem = problem;
    this->planningProblemChanged(problem);
}

//private slot
void WaypointPlannerMainWindow::handleWaysetChanged()
{
    this->ui->timelineView->setWayset(_waysetManager->wayset());
    this->ui->timelineView->setUAVParameters(_problem->uavParameters());
    this->handleWaysetSelectionChanged();
    this->enableDisableFlightActions();
}

//private slot
void WaypointPlannerMainWindow::handleWaysetSelectionChanged()
{
    this->ui->timelineView->setSelectedIndices(_waysetManager->selectedIndices());
}

void WaypointPlannerMainWindow::finishCoverageHelper()
{
    if (_coveragePolygon.isNull())
        return;

    Position startPos(_coveragePolygon->geoPoly().at(0));
    UAVOrientation startAngle(0);

    //If the start of the wayset is not empty we can do better for our starting pose
    const Wayset soFar = _waysetManager->wayset();
    if (!soFar.isEmpty())
    {
        const Position& lastPos = soFar.last().pos();

        qreal bestDist = startPos.flatDistanceEstimate(lastPos);
        Position bestPos = startPos;
        for (int i = 0; i < _coveragePolygon->geoPoly().size(); i++)
        {
            Position pos(_coveragePolygon->geoPoly().at(i));
            const qreal dist = pos.flatDistanceEstimate(lastPos);
            if (dist < bestDist)
            {
                bestDist = dist;
                bestPos = pos;
            }
        }
        startPos = bestPos;

        startAngle = lastPos.angleTo(startPos);
    }


    QSharedPointer<FlightTaskArea> area(new FlightTaskArea(_coveragePolygon->geoPoly()));
    QSharedPointer<CoverageTask> task(new CoverageTask());
    SubFlightPlanner planner(_problem->uavParameters(),
                             task,
                             area,
                             startPos,
                             startAngle);
    planner.plan();

    const Wayset& result = planner.results();
    foreach(const UAVPose& pose, result.poses())
        _waysetManager->appendWaypoint(pose.pos(), pose.angle());

    _coveragePolygon->deleteLater();
}

//private slot
void WaypointPlannerMainWindow::openProblem(const QString &filePath)
{
    _problem = CommonFileHandling::readProblemFromFile(this, filePath);

    if (_problem.isNull())
        _problem = QSharedPointer<PlanningProblem>(new PlanningProblem());

    //Reset things
    this->on_actionReset_Flight_triggered();


    _waysetManager->setPlanningProblem(_problem);
}

//private slot
void WaypointPlannerMainWindow::doInitialMapCentering()
{
    _view->setZoomLevel(16);
    _view->centerOn(-111.649225, 40.249684);
}

//private slot
void WaypointPlannerMainWindow::handleMapClick(QPoint pos)
{
    const QPointF scenePos = _view->mapToScene(pos);
    if (_mouseMode == CreateMode)
        _waysetManager->appendWaypoint(Position(scenePos));
    else if (_mouseMode == CoverageHelperMode)
    {
        if (_coveragePolygon.isNull())
        {
            QPolygonF poly;
            poly << scenePos;
            _coveragePolygon = new PolygonObject(poly);
            _scene->addObject(_coveragePolygon);
        }
        else
        {
            QPolygonF poly = _coveragePolygon->geoPoly();
            poly << scenePos;
            _coveragePolygon->setGeoPoly(poly);
        }
    }
}

//private slot
void WaypointPlannerMainWindow::on_actionExit_triggered()
{
    this->close();
}

//private slot
void WaypointPlannerMainWindow::on_actionSelect_Mode_triggered()
{
    this->setMouseMode(SelectMode);
}

//private slot
void WaypointPlannerMainWindow::on_actionCreate_Mode_triggered()
{
    this->setMouseMode(CreateMode);
}

//private slot
void WaypointPlannerMainWindow::on_actionAuto_Fix_triggered()
{
    Wayset wset = _waysetManager->wayset();
    wset.optimizeAngles(_problem->uavParameters());
    _waysetManager->setWayset(wset);
}

//private slot
void WaypointPlannerMainWindow::on_actionCoverage_Helper_triggered()
{
    this->setMouseMode(CoverageHelperMode);
}

//private slot
void WaypointPlannerMainWindow::on_actionImport_Solution_triggered()
{
    bool ok;
    Wayset wayset = CommonFileHandling::doImport(ok, QString(), this);

    if (!ok)
        return;
    _waysetManager->setWayset(wayset);
}

//private slot
void WaypointPlannerMainWindow::on_actionExport_Solution_triggered()
{
    Wayset toExport = _waysetManager->wayset();
    CommonFileHandling::doExport(toExport, QString(), this);
}

//private slot
void WaypointPlannerMainWindow::on_actionReset_Flight_triggered()
{
    _waysetManager->setWayset(Wayset(), _waysetManager->lineMode());
}

//private slot
void WaypointPlannerMainWindow::on_actionTest_Flight_triggered()
{
    const Wayset flight = _waysetManager->wayset();

    qreal score;
    bool timing;
    bool dependencies;
    bool success = SimulatedFlier::simulate(flight,
                                            _problem,
                                            &score, &timing, &dependencies);

    CommonWindowHandling::showFlightTestResults(this, success, score, timing, dependencies);
}

//private slot
void WaypointPlannerMainWindow::on_actionOpen_Problem_triggered()
{
    const QString filePath = CommonWindowHandling::getOpenProblemFilename(this);
    this->openProblem(filePath);
}

//private
void WaypointPlannerMainWindow::initProblem()
{
    _problem = QSharedPointer<PlanningProblem>(new PlanningProblem());
}

//private
void WaypointPlannerMainWindow::initMap()
{
    _scene = new MapGraphicsScene(this);
    _view = this->ui->mapView;
    _view->setScene(_scene);

    QSharedPointer<MapTileSource> tileSource(new OSMTileSource(OSMTileSource::MapQuestOSMTiles));
    _view->setTileSource(tileSource);

    connect(_view,
            SIGNAL(mapClicked(QPoint)),
            this,
            SLOT(handleMapClick(QPoint)));

    _waysetManager = new WaysetDisplayManager(_scene, _problem, Waypoint::DubinLineMode, this);
    connect(_waysetManager,
            SIGNAL(waysetChanged()),
            this,
            SLOT(handleWaysetChanged()));
    connect(_waysetManager,
            SIGNAL(waysetSelectionsChanged(QSet<int>)),
            this,
            SLOT(handleWaysetSelectionChanged()));

    this->setMouseMode(CreateMode);
}

//private slot
void WaypointPlannerMainWindow::enableDisableFlightActions()
{
    const bool hasFlight = !_waysetManager->wayset().isEmpty();

    this->ui->actionTest_Flight->setEnabled(hasFlight);
    this->ui->actionExport_Solution->setEnabled(hasFlight);
}
