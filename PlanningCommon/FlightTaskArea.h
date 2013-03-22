#ifndef FLIGHTTASKAREA_H
#define FLIGHTTASKAREA_H

#include <QObject>
#include <QPolygonF>
#include <QSharedPointer>
#include <QSet>
#include <QString>

#include "FlightTasks/FlightTask.h"
#include "Serializable.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT FlightTaskArea : public QObject, public Serializable
{
    Q_OBJECT
public:
    FlightTaskArea();
    explicit FlightTaskArea(const QPolygonF& geoPoly);

    //for de-serializing
    FlightTaskArea(QDataStream& stream);

    //pure-virtual from Serializable
    virtual QString serializationType() const;

    //pure-virtual from Serializable
    virtual void serialize(QDataStream& stream) const;

    const QPolygonF& geoPoly() const;

    const QList<QSharedPointer<FlightTask> >& tasks() const;
    void addTask(QSharedPointer<FlightTask> task);
    void removeTask(QSharedPointer<FlightTask> task);
    int numTasks() const;

    const QString& areaName() const;

    
signals:
    void flightTaskAreaChanged();
    void geoPolyChanged(const QPolygonF& geoPoly);
    void taskChanged(QSharedPointer<FlightTask> task);
    void taskAboutToAdd();
    void taskAdded(QSharedPointer<FlightTask> task);
    void taskAboutToRemove(int index);
    void taskRemoved(QSharedPointer<FlightTask> task);
    void flightTaskAreaNameChanged();
    
public slots:
    void setGeoPoly(const QPolygonF& nPoly);
    void setAreaName(const QString& nName);

private:
    QPolygonF _geoPoly;
    QList<QSharedPointer<FlightTask> > _tasks;

    QString _areaName;
    
};

#endif // FLIGHTTASKAREA_H
