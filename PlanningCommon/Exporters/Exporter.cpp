#include "Exporter.h"

#include <QFileInfo>

#include "GPXExporter.h"
#include "BinaryExporter.h"

Exporter::Exporter()
{
}

Exporter::~Exporter()
{
}

//static
QList<QString> Exporter::supportedFileTypes()
{
    QList<QString> toRet;
    toRet.append("wst");
    toRet.append("gpx");
    return toRet;
}

//static
Exporter *Exporter::getExporter(const QString &filePath)
{
    Exporter * toRet = 0;

    const QFileInfo info(filePath);
    const QString suffix = info.completeSuffix().toLower();

    if (suffix == "gpx")
        toRet = new GPXExporter(filePath);
    else if (suffix == "wst")
        toRet = new BinaryExporter(filePath);


    return toRet;
}
