#include "WaypointPlannerMainWindow.h"
#include "ui_WaypointPlannerMainWindow.h"

#include "tileSources/OSMTileSource.h"
#include "CircleObject.h"

#include <QSettings>
#include <QDesktopWidget>
#include <QtDebug>
#include <QTimer>

const QString SETTINGS_GEOMETRY = "lastGeometry";
const QString SETTINGS_WINDOWSTATE = "lastWindowState";

WaypointPlannerMainWindow::WaypointPlannerMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WaypointPlannerMainWindow)
{
    ui->setupUi(this);
    this->restoreGeometry();

    this->initMap();

    //A hack to make initial map centering work right.
    QTimer::singleShot(1, this, SLOT(doInitialMapCentering()));
}

WaypointPlannerMainWindow::~WaypointPlannerMainWindow()
{
    this->storeGeometry();
    delete ui;
}

//public slot
void WaypointPlannerMainWindow::setMouseMode(WaypointPlannerMainWindow::MouseMode mode)
{
    _mouseMode = mode;

    bool select, create;

    switch(_mouseMode)
    {
    case SelectMode:
        select = true;
        create = false;
        break;

    case CreateMode:
        select = false;
        create = true;
        break;
    }

    this->ui->actionSelect_Mode->setChecked(select);
    this->ui->actionCreate_Mode->setChecked(create);

    _waysetManager->enableMouseInteraction(mode == SelectMode);
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
    if (_mouseMode != CreateMode)
        return;

    const QPointF scenePos = _view->mapToScene(pos);
    _waysetManager->appendWaypoint(Position(scenePos));
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

//private
void WaypointPlannerMainWindow::restoreGeometry()
{
    QSettings settings;
    if (!settings.contains(SETTINGS_GEOMETRY))
        return;

    const QRect rect = settings.value(SETTINGS_GEOMETRY).toRect();

    QDesktopWidget * desktop = QApplication::desktop();
    bool onScreen = false;
    for (int i = 0; i < desktop->numScreens() && !onScreen; i++)
    {
        if (desktop->availableGeometry(i).contains(rect))
            onScreen = true;
    }
    if (onScreen)
        this->setGeometry(rect);
    else
        qDebug() << "Can't restore geometry off-screen";

    if (!settings.contains(SETTINGS_WINDOWSTATE))
        return;

    bool ok;
    const int wStateInt = settings.value(SETTINGS_WINDOWSTATE).toInt(&ok);
    const Qt::WindowStates wState = wStateInt;
    if (ok)
        this->setWindowState(wState);
}

//private
void WaypointPlannerMainWindow::storeGeometry()
{
    QSettings settings;

    const QRect rect = this->geometry();
    settings.setValue(SETTINGS_GEOMETRY, rect);

    const Qt::WindowStates wState = this->windowState();
    const int wStateInt = wState;
    settings.setValue(SETTINGS_WINDOWSTATE, wStateInt);
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

    _waysetManager = new WaysetManager(_scene, this);

    this->setMouseMode(CreateMode);
}
