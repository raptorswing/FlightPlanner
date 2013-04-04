#include "BinaryExporter.h"

#include <QDataStream>
#include <QFile>
#include <QtDebug>

BinaryExporter::BinaryExporter(const QString &filePath) :
    _filePath(filePath)
{
}

//pure-virtual from Exporter
bool BinaryExporter::doExport(const Wayset &toExport)
{
    QFile fp(_filePath);
    if (!fp.open(QFile::WriteOnly | QFile::Truncate))
    {
        qWarning() << "Failed to open" << _filePath << "for binary export";
        return false;
    }

    QDataStream stream(&fp);

    toExport.serialize(stream);

    return true;
}
