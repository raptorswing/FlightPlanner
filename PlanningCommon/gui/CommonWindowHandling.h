#ifndef COMMONWINDOWHANDLING_H
#define COMMONWINDOWHANDLING_H

class QSettings;
class QMainWindow;
class QWidget;

#include <QList>

#include "PlanningCommon_global.h"
#include "SimulatedFlier.h"

class PLANNINGCOMMONSHARED_EXPORT CommonWindowHandling
{
public:
    static void storeGeometry(QMainWindow * windowIn);
    static void restoreGeometry(QMainWindow * windowIn);
    static void showFlightTestResults(QWidget * parent,
                                      SimulatedFlierResults results);
    static void simulateFlightAndShowResults(QWidget * parent,
                                             const Wayset& wayset,
                                             const QSharedPointer<PlanningProblem>& problem);

    static QString getOpenProblemFilename(QWidget * parent);
    static QString getSaveProblemFilename(QWidget * parent);

    static QString getImportSolutionFilename(QWidget * parent);
    static QString getExportSolutionFilename(QWidget * parent);

private:
    static QString importExportFilterString(const QList<QString>& suffixes);
};

#endif // COMMONWINDOWHANDLING_H
