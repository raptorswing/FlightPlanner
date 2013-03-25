#ifndef IMPORTER_H
#define IMPORTER_H

#include "Wayset.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT Importer
{
public:
    Importer();
    virtual ~Importer();

    virtual bool doImport()=0;

    const Wayset& results() const;

protected:
    void setResults(const Wayset& results);

private:
    Wayset _results;
};

#endif // IMPORTER_H
