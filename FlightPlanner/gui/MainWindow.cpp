#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QtDebug>
#include <QMessageBox>
#include <QFileDialog>

#include "MapGraphicsView.h"
#include "MapGraphicsScene.h"
#include "tileSources/OSMTileSource.h"

#include "HierarchicalPlanner/HierarchicalPlanner.h"

#include "UAVParametersWidget.h"

#include "Exporters/GPXExporter.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _view(0), _scene(0),
    _planner(0), _viewAdapter(0)
{
    ui->setupUi(this);

    this->initMap();
    this->initPlanningProblem();
    this->initPaletteConnections();
    this->initPlanningControlConnections();
}

MainWindow::~MainWindow()
{
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
    const QString fileToWrite = QFileDialog::getSaveFileName(this,
                                                             "Select destination",
                                                             QString(),
                                                             "GPX (*.gpx);;");
    if (fileToWrite.isEmpty())
        return;

    const QFileInfo info(fileToWrite);
    const QString suffix = info.suffix().toLower();
    const QList<Position>& solution = _planner->bestFlightSoFar();

    Exporter * exporter = 0;
    if (suffix == "gpx")
        exporter = new GPXExporter(solution);
    else
    {
        QMessageBox::warning(this, "Invalid File Type", "Can't export to " + suffix + " file");
        return;
    }

    QByteArray outBytes;
    if (!exporter->doExport(&outBytes))
    {
        QMessageBox::warning(this, "Export Failed", "Unable to export due to unknown error.");
        return;
    }


    QFile fp(fileToWrite);
    if (!fp.open(QFile::WriteOnly))
    {
        QMessageBox::warning(this,"Export Failed", "Failed to open export file for writing.");
        return;
    }
    const qint64 written = fp.write(outBytes);
    if (written != outBytes.size())
    {
        QMessageBox::warning(this, "Export Failed", "Failed to write all bytes.");
        return;
    }
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
    foreach(MapGraphicsObject * obj, _displayedPathObjects)
        obj->deleteLater();
    _displayedPathObjects.clear();

    const QList<Position>& path = _planner->bestFlightSoFar();
    foreach(const Position& pos, path)
    {
        CircleObject * obj = new CircleObject(5.0, true, QColor(255,255,0));
        obj->setPos(pos.lonLat());
        obj->setZValue(100.0);
        _scene->addObject(obj);
        _displayedPathObjects.append(obj);
    }
}
