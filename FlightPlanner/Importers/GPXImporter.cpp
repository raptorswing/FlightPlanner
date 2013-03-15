#include "GPXImporter.h"

#include <QtDebug>

#include "GPX.h"
#include "Position.h"

GPXImporter::GPXImporter(const QString &filename) : _filename(filename)
{
}

//pure-virtual from Importer
bool GPXImporter::doImport()
{
    GPX gpx;
    QString errorMsg;
    if (!GPX::parseGPXFile(_filename, &gpx, &errorMsg))
    {
        qDebug() << "Failed to import soltion from gpx file. Message:" << errorMsg;
        return false;
    }

    QList<Position> toResult;
    foreach(const GPXPoint& p, gpx.points())
        toResult.append(Position(p.longitude, p.latitude, p.height));
    qDebug() << "Imported" << toResult.size() << "gpx points.";

    this->setResults(toResult);
    return true;
}
