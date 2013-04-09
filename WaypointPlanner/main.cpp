#include "gui/MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("DinoSoft");
    QCoreApplication::setApplicationName("WaypointPlanner");

    const QStringList args = a.arguments();

    WaypointPlannerMainWindow w;
    w.show();

    if (args.size() > 1)
        w.openProblem(args.at(1));
    
    return a.exec();
}
