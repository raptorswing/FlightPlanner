#include "WaypointPlannerMainWindow.h"
#include "ui_WaypointPlannerMainWindow.h"

WaypointPlannerMainWindow::WaypointPlannerMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WaypointPlannerMainWindow)
{
    ui->setupUi(this);
}

WaypointPlannerMainWindow::~WaypointPlannerMainWindow()
{
    delete ui;
}
