#include "Importer.h"

Importer::Importer()
{
}

Importer::~Importer()
{
}

const QList<Position> &Importer::results() const
{
    return _results;
}

//protected
void Importer::setResults(const QList<Position> &results)
{
    _results = results;
}
