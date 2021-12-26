#include "UAVPose.h"

UAVPose::UAVPose()
{
}

UAVPose::UAVPose(const Position &pos, const UAVOrientation &angle) :
    _pos(pos), _angle(angle)
{
}

//for de-serializing
UAVPose::UAVPose(QDataStream &stream)
{
    stream >> _pos;
    _angle = UAVOrientation(stream);
}

UAVPose::~UAVPose()
{
}

const Position &UAVPose::pos() const
{
    return _pos;
}

const UAVOrientation &UAVPose::angle() const
{
    return _angle;
}

void UAVPose::setPos(const Position &nPos)
{
    _pos = nPos;
}

void UAVPose::setAngle(const UAVOrientation &nAngle)
{
    _angle = nAngle;
}

//pure-virtual from Serializable
QString UAVPose::serializationType() const
{
    return "UAVPose";
}

//pure-virtual from Serializable
void UAVPose::serialize(QDataStream &stream) const
{
    stream << this->pos();
    this->angle().serialize(stream);
}

bool UAVPose::operator ==(const UAVPose &other) const
{
    return (_pos == other._pos
            && _angle == other._angle);
}

bool UAVPose::operator !=(const UAVPose &other) const
{
    return !(*this == other);
}

//non-member
QDebug operator<<(QDebug dbg, const UAVPose& pose)
{
    dbg.nospace() << pose.pos() << pose.angle().degrees();
    return dbg;
}
