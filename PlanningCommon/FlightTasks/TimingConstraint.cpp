#include "TimingConstraint.h"

TimingConstraint::TimingConstraint()
{
}


TimingConstraint::TimingConstraint(int allowedStartSeconds, int allowedEndSeconds) :
    _start(allowedStartSeconds), _end(allowedEndSeconds)
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

//non-member
QDataStream& operator<<(QDataStream& stream, const TimingConstraint& a)
{
    stream << a.start();
    stream << a.end();
    return stream;
}

//non-member
QDataStream& operator>>(QDataStream& stream, TimingConstraint& a)
{
    int start, end;
    stream >> start;
    stream >> end;

    a = TimingConstraint(start, end);
    return stream;
}
