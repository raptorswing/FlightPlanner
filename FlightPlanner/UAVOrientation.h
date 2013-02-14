#ifndef UAVORIENTATION_H
#define UAVORIENTATION_H

#include <QtGlobal>
#include <QtDebug>

class UAVOrientation
{
public:
    UAVOrientation();
    UAVOrientation(qreal radians);

    qreal radians() const;
    qreal degrees() const;

    bool operator ==(const UAVOrientation& other) const;
    bool operator !=(const UAVOrientation& other) const;

    void setRadians(qreal rad);

    static UAVOrientation average(const UAVOrientation& a, const UAVOrientation& b);

private:
    qreal _radians;
};

QDebug operator<<(QDebug dbg, const UAVOrientation& pos);

#endif // UAVORIENTATION_H
