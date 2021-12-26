#include "BinaryImporter.h"

#include <QFile>
#include <QtDebug>

BinaryImporter::BinaryImporter(const QString &filePath) : _filePath(filePath)
{
}

//pure-virtual from Importer
bool BinaryImporter::doImport()
{
    QFile fp(_filePath);
    if (!fp.open(QFile::ReadOnly))
    {
        qWarning() << "Failed to open" << _filePath << "for binary import";
        return false;
    }

    QDataStream stream(&fp);
    Wayset result(stream);

    this->setResults(result);
    return true;
}
