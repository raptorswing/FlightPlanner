#include "Fitness.h"

Fitness::Fitness() :
    _taskFitness(-1.0), _efficiencyFitness(-1.0)
{
}

Fitness::Fitness(qreal taskFitness, qreal efficiencyFitness) :
    _taskFitness(taskFitness), _efficiencyFitness(efficiencyFitness)
{
}

Fitness::Fitness(const Fitness &other)
{
    _taskFitness = other._taskFitness;
    _efficiencyFitness = other._efficiencyFitness;
}

Fitness &Fitness::operator =(const Fitness &other)
{
    if (&other == this)
        return *this;
    _taskFitness = other._taskFitness;
    _efficiencyFitness = other._efficiencyFitness;
    return *this;
}

bool Fitness::operator ==(const Fitness &other) const
{
    return (_taskFitness == other._taskFitness
            && _efficiencyFitness == other._efficiencyFitness);
}

bool Fitness::operator !=(const Fitness &other) const
{
    return !(other == *this);
}

bool Fitness::operator >(const Fitness &other) const
{
    if (_taskFitness > other._taskFitness)
        return true;
    else if (_taskFitness == other._taskFitness)
    {
        if (_efficiencyFitness > other._efficiencyFitness)
            return true;
    }
    return false;
}

bool Fitness::operator <(const Fitness &other) const
{
    return (other > *this);
}

bool Fitness::operator >=(const Fitness &other) const
{
    return (*this > other || *this == other);
}

bool Fitness::operator <=(const Fitness &other) const
{
    return (other > *this || *this == other);
}

qreal Fitness::taskScore() const
{
    return _taskFitness;
}

qreal Fitness::efficiencyScore() const
{
    return _efficiencyFitness;
}

qreal Fitness::combined() const
{
    return _efficiencyFitness + _taskFitness;
}

void Fitness::setTaskScore(qreal nTask)
{
    _taskFitness = nTask;
}

void Fitness::setEfficiencyScore(qreal nEfficiency)
{
    _efficiencyFitness = nEfficiency;
}
