#ifndef IMPORTER_H
#define IMPORTER_H

#include <QList>
#include "Position.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT Importer
{
public:
    Importer();
    virtual ~Importer();

    virtual bool doImport()=0;

    const QList<Position>& results() const;

protected:
    void setResults(const QList<Position>& results);

private:
    QList<Position> _results;
};

#endif // IMPORTER_H
