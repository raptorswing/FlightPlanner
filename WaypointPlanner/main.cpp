#include "WaypointPlannerMainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WaypointPlannerMainWindow w;
    w.show();
    
    return a.exec();
}
