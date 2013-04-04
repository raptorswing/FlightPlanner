#include "Importer.h"

#include <QDir>
#include <QFileInfo>
#include <QtDebug>

#include "GPXImporter.h"
#include "BinaryImporter.h"

Importer::Importer()
{
}

Importer::~Importer()
{
}

const Wayset &Importer::results() const
{
    return _results;
}

//static
QList<QString> Importer::supportedFileTypes()
{
    QList<QString> toRet;
    toRet.append("wst");
    toRet.append("gpx");
    return toRet;
}

Importer *Importer::getImporter(const QString &filePath)
{
    Importer * toRet = 0;

    const QFileInfo info(filePath);
    const QString suffix = info.completeSuffix().toLower();

    if (suffix == "gpx")
        toRet = new GPXImporter(filePath);
    else if (suffix == "wst")
        toRet = new BinaryImporter(filePath);
    else
        qWarning() << "Can't find importer for unknown filetype" << suffix;

    return toRet;
}

//protected
void Importer::setResults(const Wayset &results)
{
    _results = results;
}
