#ifndef UAVPOSE_H
#define UAVPOSE_H

#include "Position.h"
#include "UAVOrientation.h"
#include "Serializable.h"

#include <QtDebug>

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT UAVPose : public Serializable
{
public:
    UAVPose();
    UAVPose(const Position& pos, const UAVOrientation& angle);

    //for de-serializing
    UAVPose(QDataStream& stream);
    virtual ~UAVPose();

    const Position& pos() const;
    const UAVOrientation& angle() const;

    void setPos(const Position& nPos);
    void setAngle(const UAVOrientation& nAngle);

    //pure-virtual from Serializable
    virtual QString serializationType() const;
    virtual void serialize(QDataStream& stream) const;

private:
    Position _pos;
    UAVOrientation _angle;
};

QDebug operator<<(QDebug dbg, const UAVPose& pose);

#endif // UAVPOSE_H
