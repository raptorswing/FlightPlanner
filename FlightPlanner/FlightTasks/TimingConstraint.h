#ifndef TIMINGCONSTRAINT_H
#define TIMINGCONSTRAINT_H

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

#endif // TIMINGCONSTRAINT_H
