#include <QApplication>
#include <QtDebug>

#include "gui/MainWindow.h"

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
    if (args.size() > 1)
        w->openHiddenProblem(args.at(1));
    
    return a.exec();
}
