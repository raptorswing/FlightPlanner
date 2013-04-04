#ifndef COMMONFILEHANDLING_H
#define COMMONFILEHANDLING_H

#include <QWidget>
#include <QSharedPointer>

#include "Importers/Importer.h"
#include "Exporters/Exporter.h"

#include "PlanningProblem.h"

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

    static QSharedPointer<PlanningProblem> readProblemFromFile(QWidget * parent,
                                                               const QString& filePath);
};

#endif // COMMONFILEHANDLING_H
