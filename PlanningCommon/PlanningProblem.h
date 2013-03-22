#ifndef PLANNINGPROBLEM_H
#define PLANNINGPROBLEM_H

#include <QObject>
#include <QSet>
#include <QSharedPointer>

#include "UAVOrientation.h"
#include "Position.h"
#include "FlightTaskArea.h"
#include "Fitness.h"
#include "UAVParameters.h"
#include "Serializable.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT PlanningProblem : public QObject, public Serializable
{
    Q_OBJECT
public:
    PlanningProblem();

    //for de-serializing
    PlanningProblem(QDataStream& stream);

    //pure-virtual from Serializable
    virtual QString serializationType() const;

    //pure-virtual from Serializable
    virtual void serialize(QDataStream& stream) const;

    bool startingOrientationDefined() const;
    const UAVOrientation& startingOrientation() const;
    void setStartingOrientation(const UAVOrientation & orientation);

    bool startingPositionDefined() const;
    const Position& startingPosition() const;
    void setStartingPosition(const Position& pos);

    void addTaskArea(QSharedPointer<FlightTaskArea> area);
    void addTaskArea(const QPointF& centerPos);
    void removeTaskArea(QSharedPointer<FlightTaskArea> area);

    Fitness calculateFlightPerformance(const QList<Position>& positions) const;

    const QSet<QSharedPointer<FlightTaskArea> >& areas() const;

    const UAVParameters& uavParameters() const;
    void setUAVParameters(const UAVParameters& nParams);
    
signals:
    void planningProblemChanged();

    void startingOrientationChanged(const UAVOrientation& orientation);

    void startingPositionChanged(const Position& pos);

    void flighTaskAreaAdded(const QSharedPointer<FlightTaskArea>& area);
    void flightTaskAreaRemoved(const QSharedPointer<FlightTaskArea>& area);
    
public slots:

private:
    bool _startingOrientationDefined;
    UAVOrientation _startingOrientation;

    bool _startingPositionDefined;
    Position _startingPosition;

    QSet<QSharedPointer<FlightTaskArea> > _areas;

    UAVParameters _uavParameters;
    
};

#endif // PLANNINGPROBLEM_H
