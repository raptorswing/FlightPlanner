#ifndef COMMONWINDOWHANDLING_H
#define COMMONWINDOWHANDLING_H

class QSettings;
class QMainWindow;

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT CommonWindowHandling
{
public:
    static void storeGeometry(QMainWindow * windowIn);
    static void restoreGeometry(QMainWindow * windowIn);
};

#endif // COMMONWINDOWHANDLING_H
