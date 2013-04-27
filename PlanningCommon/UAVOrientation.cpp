#include "UAVOrientation.h"

const qreal PI = 3.1415926535897932384626433832795028841971693993;

UAVOrientation::UAVOrientation()
{
    _radians = 0.0;
}

UAVOrientation::UAVOrientation(qreal angle, bool radians)
{
    if (radians)
        this->setRadians(angle);
    else
        this->setDegrees(angle);
}

//For de-serializing
UAVOrientation::UAVOrientation(QDataStream &stream)
{
    stream >> _radians;
}

//pure-virtual from Serializable
QString UAVOrientation::serializationType() const
{
    return "UAVOrientation";
}

//pure-virtual from Serializable
void UAVOrientation::serialize(QDataStream &stream) const
{
    stream << _radians;
}

qreal UAVOrientation::radians() const
{
    return _radians;
}

qreal UAVOrientation::degrees() const
{
    return _radians * (180.0 / PI);
}

QDebug operator<<(QDebug dbg, const UAVOrientation& pos)
{
    dbg.nospace() << pos.degrees() << " degrees";
    return dbg.nospace();
}

bool UAVOrientation::operator ==(const UAVOrientation &other) const
{
    return qAbs<qreal>(other.degrees() - this->degrees()) < 1.0;
}

bool UAVOrientation::operator !=(const UAVOrientation &other) const
{
    return !(other == *this);
}

void UAVOrientation::setRadians(qreal radians)
{
    while (radians > 2*PI)
        radians -= 2*PI;

    while (radians < 0.0)
        radians += 2*PI;
    _radians = radians;
}

void UAVOrientation::setDegrees(qreal deg)
{
    this->setRadians(deg * PI / 180.0);
}

UAVOrientation UAVOrientation::average(const UAVOrientation &a,const UAVOrientation &b,
                                       qreal weightA, qreal weightB)
{
    const qreal smallAngle1 = qMin<qreal>(a.radians(), b.radians());
    const qreal smallAngle2 = smallAngle1 + (2.0 * PI);
    const qreal bigAngle = qMax<qreal>(a.radians(), b.radians());

    qreal smallWeight = weightA;
    qreal bigWeight = weightB;
    if (bigAngle == a.radians())
    {
        smallWeight = weightB;
        bigWeight = weightA;
    }

    if (qAbs<qreal>(smallAngle1 - bigAngle) < qAbs<qreal>(smallAngle2 - bigAngle))
        return UAVOrientation(smallWeight * smallAngle1 + bigWeight * bigAngle);
    else
        return UAVOrientation(smallWeight * smallAngle2 + bigWeight * bigAngle);
}
