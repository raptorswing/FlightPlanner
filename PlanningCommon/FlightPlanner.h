#ifndef FLIGHTPLANNER_H
#define FLIGHTPLANNER_H

#include <QObject>
#include <QSharedPointer>
#include <QTimer>

#include "PlanningProblem.h"
#include "Fitness.h"
#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT FlightPlanner : public QObject
{
    Q_OBJECT
public:
    enum PlanningStatus
    {
        Stopped,
        Running,
        Paused
    };
public:
    explicit FlightPlanner(QSharedPointer<PlanningProblem> prob = QSharedPointer<PlanningProblem>(),
                           QObject *parent = 0);
    virtual ~FlightPlanner();

    QSharedPointer<PlanningProblem> problem() const;
    void setProblem(QSharedPointer<PlanningProblem> nProb);

    Fitness bestFitnessSoFar() const;

    const QList<Position>& bestFlightSoFar() const;
    void setBestFlightSoFar(const QList<Position>& nFlight);

    quint32 iterations() const;
    
signals:
    void plannerProgressChanged(qreal fitness, quint32 iterations);
    void plannerStatusChanged(FlightPlanner::PlanningStatus status);
    
public slots:
    void startPlanning();
    void pausePlanning();
    void resetPlanning();

protected:
    virtual void doStart()=0;
    virtual void doIteration()=0;
    virtual void doReset()=0;

    void setBestFitnessSoFar(const Fitness& nBest);

private:
    QSharedPointer<PlanningProblem> _prob;
    QTimer * _planningTimer;

    quint32 _iterations;

    Fitness _bestFitnessSoFar;
    QList<Position> _bestFlightSoFar;

private slots:
    //The timer calls this one, which will call the polymorphic instance
    void handlePlanningTimerTimeout();
    
};

#endif // FLIGHTPLANNER_H
