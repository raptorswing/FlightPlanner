#include "gui/MainWindow.h"
#include <QApplication>

#include "gui/CommonFileHandling.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("DinoSoft");
    QCoreApplication::setApplicationName("WaypointPlanner");

    const QStringList args = a.arguments();

    QString problemToOpen;
    if (args.size() >= 4)
    {
        const QString workingDirectory = args.at(1);
        const QString problem = args.at(2);
        const QString resultsPrefix = args.at(3);


        CommonFileHandling::setWorkingDirectory(workingDirectory);
        CommonFileHandling::setResultPrefix(resultsPrefix);
        problemToOpen = problem;
    }

    WaypointPlannerMainWindow * w = new WaypointPlannerMainWindow();
    w->show();
    w->setAttribute(Qt::WA_DeleteOnClose);
    if (!problemToOpen.isEmpty())
        w->openProblem(problemToOpen);
    
    return a.exec();
}
