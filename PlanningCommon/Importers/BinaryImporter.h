#ifndef BINARYIMPORTER_H
#define BINARYIMPORTER_H

#include "Importer.h"

class BinaryImporter : public Importer
{
public:
    BinaryImporter(const QString& filePath);

    //pure-virtual from Importer
    virtual bool doImport();

private:
    QString _filePath;
};

#endif // BINARYIMPORTER_H
