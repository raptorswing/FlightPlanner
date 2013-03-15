#ifndef GPXIMPORTER_H
#define GPXIMPORTER_H

#include "Importer.h"

class GPXImporter : public Importer
{
public:
    GPXImporter(const QString& filename);

    //pure-virtual from Importer
    virtual bool doImport();

private:
    QString _filename;
};

#endif // GPXIMPORTER_H
