#include "WaypointPlannerMainWindow.h"
#include "ui_WaypointPlannerMainWindow.h"

#include "tileSources/OSMTileSource.h"

#include <QSettings>
#include <QDesktopWidget>
#include <QtDebug>
const QString SETTINGS_GEOMETRY = "lastGeometry";
const QString SETTINGS_WINDOWSTATE = "lastWindowState";

WaypointPlannerMainWindow::WaypointPlannerMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WaypointPlannerMainWindow)
{
    ui->setupUi(this);
    this->restoreGeometry();

    this->initMap();
}

WaypointPlannerMainWindow::~WaypointPlannerMainWindow()
{
    this->storeGeometry();
    delete ui;
}

//private slot
void WaypointPlannerMainWindow::on_actionExit_triggered()
{
    this->close();
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
    _view = new MapGraphicsView(_scene, this);
    QSharedPointer<MapTileSource> tileSource(new OSMTileSource(OSMTileSource::MapQuestAerialTiles));
    _view->setTileSource(tileSource);
    this->setCentralWidget(_view);
}
