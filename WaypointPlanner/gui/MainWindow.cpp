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

WaypointPlannerMainWindow::WaypointPlannerMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->ui->setupUi(this);
    CommonWindowHandling::restoreGeometry(this);
    this->ui->menuView->addAction(this->ui->toolBar->toggleViewAction());

    this->initProblem();
    this->initMap();

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

    switch(_mouseMode)
    {
    case SelectMode:
        select = true;
        break;

    case CreateMode:
        create = true;
        break;

    case CoverageHelperMode:
        if (!_coveragePolygon.isNull())
            _coveragePolygon->deleteLater();
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
    _waysetManager->fixAll();
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

//private
void WaypointPlannerMainWindow::initProblem()
{
    _problem = QSharedPointer<PlanningProblem>(new PlanningProblem());
}

//private
void WaypointPlannerMainWindow::initMap()
{
    _scene = new MapGraphicsScene(this);
    _view = new WaypointMapView(_scene, this);

    QSharedPointer<MapTileSource> tileSource(new OSMTileSource(OSMTileSource::MapQuestOSMTiles));
    _view->setTileSource(tileSource);

    connect(_view,
            SIGNAL(mapClicked(QPoint)),
            this,
            SLOT(handleMapClick(QPoint)));

    this->setCentralWidget(_view);

    _waysetManager = new WaysetDisplayManager(_scene, _problem, Waypoint::DubinLineMode, this);

    this->setMouseMode(CreateMode);
}
