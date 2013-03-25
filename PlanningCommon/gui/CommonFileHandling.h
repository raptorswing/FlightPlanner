#ifndef COMMONFILEHANDLING_H
#define COMMONFILEHANDLING_H

#include <QWidget>

#include "Importers/Importer.h"
#include "Exporters/Exporter.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT CommonFileHandling
{
public:
    static bool doExport(const Wayset& toExport,
                         QString destFile = QString(),
                         QWidget * parent = 0);
    static Wayset doImport(bool &ok,
                                             QString destFile = QString(),
                                             QWidget * parent = 0);
};

#endif // COMMONFILEHANDLING_H
