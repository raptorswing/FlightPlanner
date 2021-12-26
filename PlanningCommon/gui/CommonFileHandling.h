#ifndef COMMONFILEHANDLING_H
#define COMMONFILEHANDLING_H

#include <QWidget>
#include <QSharedPointer>

#include "Importers/Importer.h"
#include "Exporters/Exporter.h"

#include "PlanningProblem.h"
#include "UserStudyChatHandler.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT CommonFileHandling
{
public:
    static bool doExport(const Wayset& toExport,
                         QString destFile = QString(),
                         QWidget * parent = 0);
    static Wayset doImport(bool &ok,
                           QString srcFile = QString(),
                           QWidget * parent = 0);

    static QSharedPointer<PlanningProblem> readProblemFromFile(QWidget * parent,
                                                               const QString& filePath);

    static bool writeChatResponseResults(UserStudyChatHandler * handler);

    static bool setWorkingDirectory(const QString& dirString);

    static void setResultPrefix(const QString& nPrefix);
    static QString resultsPrefix();

private:
    static QString _resultPrefix;
};

#endif // COMMONFILEHANDLING_H
