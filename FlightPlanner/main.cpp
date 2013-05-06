#include <QApplication>
#include <QtDebug>

#include "gui/MainWindow.h"
#include "gui/CommonFileHandling.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("DinoSoft");
    QCoreApplication::setApplicationName("FlightPlanner");

    const QStringList args = a.arguments();

    MainWindow * w = new MainWindow();
    w->show();
    w->setAttribute(Qt::WA_DeleteOnClose);

    //Open "hidden" problem if applicable
    if (args.size() >= 4)
    {
        const QString workingDirectory = args.at(1);
        const QString problem = args.at(2);
        const QString resultsPrefix = args.at(3);


        CommonFileHandling::setWorkingDirectory(workingDirectory);
        CommonFileHandling::setResultPrefix(resultsPrefix);
        w->openHiddenProblem(problem);
    }
    
    return a.exec();
}
