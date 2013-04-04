#include "GPXExporter.h"

#include "GPX.h"

GPXExporter::GPXExporter(const QString &filePath) : _filePath(filePath)
{
}

bool GPXExporter::doExport(const Wayset &wayset)
{
    QByteArray bytes;
    GPX gpx;

    foreach(const Position& pos, wayset.positions())
    {
        GPXPoint point;
        point.longitude = pos.longitude();
        point.latitude = pos.latitude();
        point.height = pos.altitude();

        gpx.appendPoint(point);
    }

    return gpx.toXML(&bytes);
}
