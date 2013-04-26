#include "GPXExporter.h"

#include "GPX.h"
#include <QFile>

GPXExporter::GPXExporter(const QString &filePath) : _filePath(filePath)
{
}

bool GPXExporter::doExport(const Wayset &wayset)
{
    QFile fp(_filePath);
    if (!fp.open(QFile::WriteOnly | QFile::Text))
    {
        qWarning() << "Failed to open" << _filePath << "for writing";
        return false;
    }

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

    QString error;
    if (!gpx.toXML(&bytes, &error))
    {
        qWarning() << "GPX export failed. Message:" << error;
        return false;
    }

    fp.write(bytes);
    return true;
}
