#ifndef ANGLERANGE_H
#define ANGLERANGE_H

#include "UAVOrientation.h"
#include "Serializable.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT AngleRange : public Serializable
{
public:
    AngleRange();
    AngleRange(const UAVOrientation& center, qreal range, bool radians = true);

    //for de-serializing
    AngleRange(QDataStream &stream);

    //pure virtual from Serializable
    virtual QString serializationType() const;

    //pure virtual from Serializable
    virtual void serialize(QDataStream& stream) const;

    bool isNull() const;

    const UAVOrientation& center() const;
    void setCenter(const UAVOrientation& newCenter);

    qreal rangeDegrees() const;
    qreal rangeRadians() const;

    void setRangeDegrees(qreal nRangeDegrees);
    void setRangeRadians(qreal nRangeRadians);

    bool withinRange(const UAVOrientation& orientation) const;

private:
    UAVOrientation _center;
    qreal _rangeRadians;
};

#endif // ANGLERANGE_H
