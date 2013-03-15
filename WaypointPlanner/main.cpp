#include "WaypointPlannerMainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("DinoSoft");
    QCoreApplication::setApplicationName("WaypointPlanner");

    WaypointPlannerMainWindow w;
    w.show();
    
    return a.exec();
}
