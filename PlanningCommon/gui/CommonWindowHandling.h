#ifndef COMMONWINDOWHANDLING_H
#define COMMONWINDOWHANDLING_H

class QSettings;
class QMainWindow;
class QWidget;

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT CommonWindowHandling
{
public:
    static void storeGeometry(QMainWindow * windowIn);
    static void restoreGeometry(QMainWindow * windowIn);
    static void showFlightTestResults(QWidget * parent,
                                      bool success, qreal score);
};

#endif // COMMONWINDOWHANDLING_H
