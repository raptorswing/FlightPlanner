#ifndef TIMINGCONSTRAINT_H
#define TIMINGCONSTRAINT_H

#include <QDataStream>

class TimingConstraint
{
public:
    TimingConstraint();
    TimingConstraint(int allowedStartSeconds,
                     int allowedEndSeconds);

    int start() const;
    int end() const;

    void setStart(int nStart);
    void setEnd(int nEnd);

    bool operator ==(const TimingConstraint& other) const;
    bool operator !=(const TimingConstraint& other) const;

private:
    int _start;
    int _end;
};

QDataStream& operator<<(QDataStream& stream, const TimingConstraint& a);
QDataStream& operator>>(QDataStream& stream, TimingConstraint& a);

#endif // TIMINGCONSTRAINT_H
