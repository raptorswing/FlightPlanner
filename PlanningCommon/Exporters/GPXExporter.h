#ifndef GPXEXPORTER_H
#define GPXEXPORTER_H

#include "Exporter.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT GPXExporter : public Exporter
{
public:
    GPXExporter(const QString& filePath);

    virtual bool doExport(const Wayset& wayset);

private:
    QString _filePath;

};

#endif // GPXEXPORTER_H
