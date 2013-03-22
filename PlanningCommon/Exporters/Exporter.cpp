#include "Exporter.h"

Exporter::Exporter(const QList<Position> &solution) : _solution(solution)
{
}

Exporter::~Exporter()
{
}

const QList<Position> &Exporter::solution() const
{
    return _solution;
}
