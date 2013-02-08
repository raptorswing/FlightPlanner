#ifndef GPXEXPORTER_H
#define GPXEXPORTER_H

#include "Exporter.h"

class GPXExporter : public Exporter
{
public:
    GPXExporter(const QList<Position>& solution);

    virtual bool doExport(QByteArray * output);
};

#endif // GPXEXPORTER_H
