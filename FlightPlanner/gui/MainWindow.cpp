#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QtDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>

#include "FancyMapGraphicsView.h"
#include "MapGraphicsScene.h"
#include "tileSources/CompositeTileSource.h"
#include "tileSources/OSMTileSource.h"

#include "HierarchicalPlanner/HierarchicalPlanner.h"
#include "SimulatedFlier.h"

#include "UAVParametersWidget.h"

#include "gui/CommonFileHandling.h"
#include "gui/CommonWindowHandling.h"
#include "MouseMetrics.h"

const QString PERF_LOG = "Performance.csv";
const QString GENERAL_LOG = "General.txt";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //GRAB ALL THE MOUSE CLICKS
    MouseMetrics::buildInstance();
    this->installEventFilter(MouseMetrics::instance());

    ui->setupUi(this);

    //Restore window geometry
    CommonWindowHandling::restoreGeometry(this);

    this->initMap();
    this->initPlanningProblem();

    qsrand(QDateTime::currentMSecsSinceEpoch());

    //Do chat messages for the user study...
    _chatHandler = new UserStudyChatHandler(this->ui->chatWidget, this);
    _eventLogger = new UserStudyEventLogger(CommonFileHandling::resultsPrefix(), this);
}

MainWindow::~MainWindow()
{

    //If we are in user study mode, write stuff out before we exit
    if (!CommonFileHandling::resultsPrefix().isEmpty())
    {
        QString filename = CommonFileHandling::resultsPrefix() % " " % QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()) % "-" % "FINAL";
        this->saveProblem(filename % ".prb");
        CommonFileHandling::doExport(_planner->bestFlightSoFar(),
                                     filename % ".wst",
                                     this);
    }

    //Store window geometry
    CommonWindowHandling::storeGeometry(this);

    //Write user study results
    CommonFileHandling::writeChatResponseResults(_chatHandler);

    UserStudyEventLogger::logMouseMetrics(_eventLogger, "clicks.csv", MouseMetrics::instance());
    MouseMetrics::destroyInstance();

    delete ui;
}

//public slot
void MainWindow::openHiddenProblem(const QString &filePath)
{
    _hiddenProblem = CommonFileHandling::readProblemFromFile(this, filePath);

    if (_hiddenProblem.isNull())
    {
        qWarning() << "Failed to open hidden problem" << filePath;
        return;
    }

    this->loadHiddenProblemAttributes();

    //Center on the starting position of the hidden problem
    if (_hiddenProblem->startingPositionDefined())
        _view->centerOn(_hiddenProblem->startingPosition().lonLat());

    qDebug() << "Hidden problem" << filePath << "opened";

    _eventLogger->addTimestampedLine(GENERAL_LOG, "Opened " + filePath);

    QStringList parts;
    _eventLogger->addTimestampedCSVLine(PERF_LOG, parts);

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
    else if (_planner->status() == FlightPlanner::Paused && _planner->bestFlightSoFar().isEmpty())
    {
        QMessageBox::warning(this,
                             "Planning Failed",
                             "The planner was unable to find a satisfactory flight. Please double-check check your task areas, tasks, and settings (such as timing and dependency constraints).");
    }

    const Wayset& bestSoFar = _planner->bestFlightSoFar();

    _waysetManager->setPlanningProblem(_problem);
    _waysetManager->setWayset(bestSoFar);

    this->enableDisableFlightActions();
}

//private slot
void MainWindow::openProblem(const QString &filePath)
{
    _problem = CommonFileHandling::readProblemFromFile(this, filePath);
    if (_problem.isNull())
    {
        qWarning() << "Failed to open problem" << filePath;
        return;
    }

    _planner->setProblem(_problem);
    _viewAdapter->setModel(_problem);

    connect(_problem.data(),
            SIGNAL(planningProblemChanged()),
            _planner,
            SLOT(resetPlanning()));

    this->loadHiddenProblemAttributes();

    if (_problem->startingPositionDefined())
        _view->centerOn(_problem->startingPosition().lonLat());
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

    this->loadHiddenProblemAttributes();
}

//private slot
void MainWindow::handleFlightTaskAreaDrawn(const QSharedPointer<FlightTaskArea> &area)
{
    _problem->addTaskArea(area);
}

//private slot
void MainWindow::on_actionOpen_triggered()
{
    const QString filePath = CommonWindowHandling::getOpenProblemFilename(this);
    if (filePath.isEmpty())
        return;

    this->openProblem(filePath);
}

//private slot
void MainWindow::on_actionSave_Planning_Problem_triggered()
{
    if (_problem.isNull())
        return;

    const QString filePath = CommonWindowHandling::getSaveProblemFilename(this);
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
    CommonFileHandling::doExport(solution, QString(), this);
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

//private slot
void MainWindow::on_actionTest_Flight_triggered()
{
    //We use the "hidden" problem for judging flights, if available
    QSharedPointer<PlanningProblem> problemToUse = _problem;
    if (!_hiddenProblem.isNull())
        problemToUse = _hiddenProblem;

    const SimulatedFlierResults results = SimulatedFlier::simulate(_planner->bestFlightSoFar(),
                                                                   problemToUse);
    CommonWindowHandling::showFlightTestResults(this, results);

    //If we are in user study mode, write stuff out when we test
    if (!CommonFileHandling::resultsPrefix().isEmpty())
    {
        QString filename = CommonFileHandling::resultsPrefix() % " " % QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()) % "-" % QString::number(results.points / results.pointsPossible);
        this->saveProblem(filename % ".prb");
        CommonFileHandling::doExport(_planner->bestFlightSoFar(),
                                     filename % ".wst",
                                     this);
    }

    const qreal lengthMeters = _planner->bestFlightSoFar().lengthMeters(_problem->uavParameters());
    UserStudyEventLogger::logFlightPerformance(_eventLogger,
                                               PERF_LOG,
                                               results,
                                               lengthMeters);
}


//private
void MainWindow::initMap()
{
    //Setup MapGraphicsScene and View
    _scene = new MapGraphicsScene(this);

    FancyMapGraphicsView * fancy = new FancyMapGraphicsView(_scene, this);
    connect(fancy,
            SIGNAL(flightTaskAreaDrawn(QSharedPointer<FlightTaskArea>)),
            this,
            SLOT(handleFlightTaskAreaDrawn(QSharedPointer<FlightTaskArea>)));
    _view = fancy;

    this->setCentralWidget(_view);

    //Setup Map tile sources
    QSharedPointer<CompositeTileSource> composite(new CompositeTileSource());
    QSharedPointer<MapTileSource> osm(new OSMTileSource());
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
//    connect(_planner.data(),
//            SIGNAL(plannerPaused()),
//            this,
//            SLOT(updateDisplayedFlight()));

    _viewAdapter = new ProblemViewAdapter(_problem,
                                          _scene,
                                          this);

    _waysetManager = new WaysetDisplayManager(_scene, _problem, Waypoint::DubinLineMode, this);
    _waysetManager->enableMouseInteraction(false);

    this->enableDisableFlightActions();
}

//private
void MainWindow::enableDisableFlightActions()
{
    bool hasFlight = false;
    if (!_planner->bestFlightSoFar().isEmpty())
        hasFlight = true;

    this->ui->actionExport_Solution->setEnabled(hasFlight);
    this->ui->actionReset_Flight->setEnabled(hasFlight);
    this->ui->actionTest_Flight->setEnabled(hasFlight);
}

//private
void MainWindow::loadHiddenProblemAttributes()
{
    if (_hiddenProblem.isNull())
        return;
    else if (_problem.isNull())
        return;

    _problem->setUAVParameters(_hiddenProblem->uavParameters());

    if (_hiddenProblem->startingPositionDefined())
        _problem->setStartingPosition(_hiddenProblem->startingPosition());

    if (_hiddenProblem->startingOrientationDefined())
        _problem->setStartingOrientation(_hiddenProblem->startingOrientation());

}
