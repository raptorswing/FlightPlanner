#ifndef FITNESS_H
#define FITNESS_H

#include <QtGlobal>

class Fitness
{
public:
    Fitness();
    Fitness(qreal taskFitness, qreal efficiencyFitness);
    Fitness(const Fitness& other);

    Fitness& operator =(const Fitness& other);

    bool operator ==(const Fitness& other) const;
    bool operator !=(const Fitness& other) const;
    bool operator >(const Fitness& other) const;
    bool operator <(const Fitness& other) const;
    bool operator >=(const Fitness& other) const;
    bool operator <=(const Fitness& other) const;

    qreal taskScore() const;
    qreal efficiencyScore() const;
    qreal combined() const;

    void setTaskScore(qreal nTask);
    void setEfficiencyScore(qreal nEfficiency);

private:
    qreal _taskFitness;
    qreal _efficiencyFitness;
};

#endif // FITNESS_H
