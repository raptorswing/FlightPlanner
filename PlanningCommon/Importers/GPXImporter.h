#ifndef GPXIMPORTER_H
#define GPXIMPORTER_H

#include "Importer.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT GPXImporter : public Importer
{
public:
    GPXImporter(const QString& filename);

    //pure-virtual from Importer
    virtual bool doImport();

private:
    QString _filename;
};

#endif // GPXIMPORTER_H
