#ifndef WAYSET_H
#define WAYSET_H

#include <QList>

#include "UAVParameters.h"
#include "UAVPose.h"
#include "Dubins.h"
#include "Serializable.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT Wayset : public Serializable
{
public:
    Wayset();
    Wayset(const QList<UAVPose>& poses);

    //For de-serialization
    Wayset(QDataStream& stream);

    const UAVPose& at(int i) const;

    const UAVPose& first() const;
    const UAVPose& last() const;

    int count() const;
    int size() const;
    bool isEmpty() const;

    bool contains(const UAVPose& pose) const;
    bool contains(const Position& pos) const;

    qreal lengthMeters(const UAVParameters& uavParams) const;
    QList<qreal> waypointSubLengthsMeters(const UAVParameters& uavParams) const;
    qreal timeToFly(const UAVParameters& uavParams) const;
    Wayset resample(qreal granularityMeters, const UAVParameters& uavParams) const;
    QList<Dubins> dubins(const UAVParameters& uavParams) const;
    void optimizeAngles(const UAVParameters& uavParams);

    UAVPose sampleAtTime(qreal desiredTime, const UAVParameters& uavParams) const;
    UAVPose sampleAtDistance(qreal distance, const UAVParameters& uavParams) const;

    qreal distToPose(const UAVPose& pose, const UAVParameters& uavParams) const;
    qreal distToPoseIndex(int index, const UAVParameters& uavParams) const;

    Wayset portionByDist(qreal startDist, qreal endDist, const UAVParameters& uavParams) const;
    Wayset portionByTime(qreal startTime, qreal endTime, const UAVParameters& uavParams) const;
    Wayset discretePortionByDist(qreal startDist, qreal endDist, const UAVParameters& uavParams) const;
    Wayset discretePortionByTime(qreal startTime, qreal endTime, const UAVParameters& uavParams) const;

    void cleanup();
    void clear();

    void append(const UAVPose& pose);
    void append(const Wayset& wayset);
    void append(const Position& pos,
                const UAVOrientation& angle = UAVOrientation());

    void prepend(const UAVPose& pose);
    void prepend(const Position& pos,
                 const UAVOrientation& angle = UAVOrientation());

    const QList<UAVPose>& poses() const;
    QList<Position> positions() const;
    QList<UAVOrientation> angles() const;

    //pure-virtual from Serializable
    virtual QString serializationType() const;

    //pure-virtual from Serializable
    virtual void serialize(QDataStream& stream) const;

private:
    QList<UAVPose> _poses;
};

#endif // WAYSET_H
