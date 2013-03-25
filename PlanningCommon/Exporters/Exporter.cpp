#include "Exporter.h"

Exporter::Exporter(const Wayset &solution) : _solution(solution)
{
}

Exporter::~Exporter()
{
}

const Wayset &Exporter::solution() const
{
    return _solution;
}
