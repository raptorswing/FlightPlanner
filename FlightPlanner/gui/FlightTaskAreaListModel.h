#ifndef FLIGHTTASKAREALISTMODEL_H
#define FLIGHTTASKAREALISTMODEL_H

#include <QAbstractListModel>
#include <QWeakPointer>
#include <QObject>

#include "FlightTaskArea.h"

class FlightTaskAreaListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit FlightTaskAreaListModel(QWeakPointer<FlightTaskArea> area, QObject * parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    QWeakPointer<FlightTaskArea> _area;
    
signals:
    
public slots:
    void handleRowsAboutToAdd();
    void handleRowsAdded();

    void handleRowAboutToRemove(int index);
    void handleRowRemoved();
    
};

#endif // FLIGHTTASKAREALISTMODEL_H
