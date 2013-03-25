#ifndef GPXEXPORTER_H
#define GPXEXPORTER_H

#include "Exporter.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT GPXExporter : public Exporter
{
public:
    GPXExporter(const Wayset& solution);

    virtual bool doExport(QByteArray * output);
};

#endif // GPXEXPORTER_H
