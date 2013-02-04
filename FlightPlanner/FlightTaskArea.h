#ifndef FLIGHTTASKAREA_H
#define FLIGHTTASKAREA_H

#include <QObject>
#include <QPolygonF>
#include <QSharedPointer>
#include <QSet>

#include "FlightTasks/FlightTask.h"

class FlightTaskArea : public QObject
{
    Q_OBJECT
public:
    FlightTaskArea();
    explicit FlightTaskArea(const QPolygonF& geoPoly);

    const QPolygonF& geoPoly() const;

    const QList<QSharedPointer<FlightTask> >& tasks() const;
    void addTask(QSharedPointer<FlightTask> task);
    void removeTask(QSharedPointer<FlightTask> task);
    int numTasks() const;

    
signals:
    void flightTaskAreaChanged();
    void geoPolyChanged(const QPolygonF& geoPoly);
    void taskChanged(QSharedPointer<FlightTask> task);
    void taskAboutToAdd();
    void taskAdded(QSharedPointer<FlightTask> task);
    void taskRemoved(QSharedPointer<FlightTask> task);
    
public slots:
    void setGeoPoly(const QPolygonF& nPoly);

private:
    QPolygonF _geoPoly;
    QList<QSharedPointer<FlightTask> > _tasks;
    
};

#endif // FLIGHTTASKAREA_H
