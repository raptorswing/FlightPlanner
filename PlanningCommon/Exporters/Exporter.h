#ifndef EXPORTER_H
#define EXPORTER_H

#include "Wayset.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT Exporter
{
public:
    Exporter(const Wayset& solution);
    virtual ~Exporter();

    const Wayset& solution() const;

    virtual bool doExport(QByteArray * output)=0;

private:
    Wayset _solution;
};

#endif // EXPORTER_H
