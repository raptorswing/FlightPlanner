#ifndef UAVORIENTATION_H
#define UAVORIENTATION_H

#include <QtGlobal>
#include <QtDebug>

#include "Serializable.h"

class UAVOrientation
{
public:
    UAVOrientation();
    UAVOrientation(qreal radians);

    //for de-serializing
    UAVOrientation(QDataStream& stream);

    //pure-virtual from Serializable
    virtual QString serializationKey() const;

    //pure-virtual from Serializable
    virtual void serialize(QDataStream& stream) const;

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
