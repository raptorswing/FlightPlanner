#ifndef BINARYEXPORTER_H
#define BINARYEXPORTER_H

#include "Exporter.h"

class BinaryExporter : public Exporter
{
public:
    BinaryExporter(const QString& filePath);

    //pure-virtual from Exporter
    virtual bool doExport(const Wayset& toExport);

private:
    QString _filePath;
};

#endif // BINARYEXPORTER_H
