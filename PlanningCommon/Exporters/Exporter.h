#ifndef EXPORTER_H
#define EXPORTER_H

#include <QList>
#include "Position.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT Exporter
{
public:
    Exporter(const QList<Position>& solution);
    virtual ~Exporter();

    const QList<Position>& solution() const;

    virtual bool doExport(QByteArray * output)=0;

private:
    QList<Position> _solution;
};

#endif // EXPORTER_H
