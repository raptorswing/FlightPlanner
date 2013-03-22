#include "UAVOrientation.h"

const qreal PI = 3.1415926535897932384626433832795028841971693993;

UAVOrientation::UAVOrientation()
{
    _radians = 0.0;
}

UAVOrientation::UAVOrientation(qreal radians)
{
    this->setRadians(radians);
}

//For de-serializing
UAVOrientation::UAVOrientation(QDataStream &stream)
{
    stream >> _radians;
}

//pure-virtual from Serializable
QString UAVOrientation::serializationKey() const
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
    return qAbs<qreal>(other.radians() - this->radians()) < 0.001;
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

UAVOrientation UAVOrientation::average(const UAVOrientation &a, const UAVOrientation &b)
{
    const qreal smallAngle1 = qMin<qreal>(a.radians(), b.radians());
    const qreal smallAngle2 = smallAngle1 + (2.0 * 3.14159265);
    const qreal bigAngle = qMax<qreal>(a.radians(), b.radians());

    if (qAbs<qreal>(smallAngle1 - bigAngle) < qAbs<qreal>(smallAngle2 - bigAngle))
        return UAVOrientation((smallAngle1 + bigAngle) / 2.0);
    else
        return UAVOrientation((smallAngle2 + bigAngle) / 2.0);
}
