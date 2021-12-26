#ifndef EXPORTER_H
#define EXPORTER_H

#include "Wayset.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT Exporter
{
public:
    Exporter();
    virtual ~Exporter();

    virtual bool doExport(const Wayset& toExport)=0;

    static QList<QString> supportedFileTypes();
    static Exporter * getExporter(const QString& filePath);
};

#endif // EXPORTER_H
