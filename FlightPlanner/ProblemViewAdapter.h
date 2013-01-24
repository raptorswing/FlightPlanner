#ifndef PROBLEMVIEWADAPTER_H
#define PROBLEMVIEWADAPTER_H

#include <QObject>
#include <QSharedPointer>
#include <QSet>

#include "PlanningProblem.h"
#include "MapGraphicsScene.h"
#include "gui/StartPosMapObject.h"
#include "gui/FlightTaskAreaMapObject.h"

class ProblemViewAdapter : public QObject
{
    Q_OBJECT
public:
    explicit ProblemViewAdapter(QSharedPointer<PlanningProblem> model,
                                MapGraphicsScene * view,
                                QObject *parent = 0);
    
signals:
    
public slots:
    void setModel(QSharedPointer<PlanningProblem> model);

private slots:
    void handleStartingPositionChanged(const Position& pos);
    void handleStartingOrientationChanged(const UAVOrientation& orientation);

    void handleFlightTaskAreaAdded(const QSharedPointer<FlightTaskArea>& area);
    void handleFlightTaskAreaRemoved(const QSharedPointer<FlightTaskArea>& area);

private:
    QSharedPointer<PlanningProblem> _model;
    MapGraphicsScene * _view;

    QSharedPointer<CircleObject> _startPosObject;

    QSet<QSharedPointer<FlightTaskAreaMapObject> > _areaObjects;
    
};

#endif // PROBLEMVIEWADAPTER_H
