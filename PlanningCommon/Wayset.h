#ifndef WAYSET_H
#define WAYSET_H

#include <QList>

#include "UAVParameters.h"
#include "UAVPose.h"
#include "Dubins.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT Wayset
{
public:
    Wayset();
    Wayset(const QList<UAVPose>& poses);

    const UAVPose& at(int i) const;

    const UAVPose& first() const;
    const UAVPose& last() const;

    int count() const;
    int size() const;
    bool isEmpty() const;

    qreal lengthMeters(const UAVParameters& uavParams) const;
    qreal timeToFly(const UAVParameters& uavParams) const;
    Wayset resample(qreal granularityMeters, const UAVParameters& uavParams) const;
    QList<Dubins> dubins(const UAVParameters& uavParams) const;

    void clear();

    void append(const UAVPose& pos);
    void append(const Wayset& wayset);
    void append(const Position& pos,
                const UAVOrientation& angle = UAVOrientation());

    void prepend(const UAVPose& pos);
    void prepend(const Position& pos,
                 const UAVOrientation& angle = UAVOrientation());

    const QList<UAVPose>& poses() const;
    QList<Position> positions() const;
    QList<UAVOrientation> angles() const;

private:
    QList<UAVPose> _poses;
};

#endif // WAYSET_H
