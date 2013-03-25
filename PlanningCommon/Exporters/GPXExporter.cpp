#include "GPXExporter.h"

#include "GPX.h"

GPXExporter::GPXExporter(const Wayset& solution) : Exporter(solution)
{
}

bool GPXExporter::doExport(QByteArray *output)
{
    GPX gpx;

    foreach(const Position& pos, this->solution().waypoints())
    {
        GPXPoint point;
        point.longitude = pos.longitude();
        point.latitude = pos.latitude();
        point.height = pos.altitude();

        gpx.appendPoint(point);
    }

    return gpx.toXML(output);
}
