#include "Importer.h"

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

//protected
void Importer::setResults(const Wayset &results)
{
    _results = results;
}
