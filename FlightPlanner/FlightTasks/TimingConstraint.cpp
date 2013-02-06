#include "TimingConstraint.h"

TimingConstraint::TimingConstraint()
{
}


TimingConstraint::TimingConstraint(int disallowedStartSeconds, int disallowedEndSeconds) :
    _start(disallowedStartSeconds), _end(disallowedEndSeconds)
{
}

int TimingConstraint::start() const
{
    return _start;
}

int TimingConstraint::end() const
{
    return _end;
}

void TimingConstraint::setStart(int nStart)
{
    _start = nStart;
}

void TimingConstraint::setEnd(int nEnd)
{
    _end = nEnd;
}

bool TimingConstraint::operator ==(const TimingConstraint &other) const
{
    return (other.start() == this->start()
            && other.end() == this->end());
}

bool TimingConstraint::operator !=(const TimingConstraint &other) const
{
    return !(other == *this);
}
