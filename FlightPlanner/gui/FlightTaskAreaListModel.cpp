#include "FlightTaskAreaListModel.h"

FlightTaskAreaListModel::FlightTaskAreaListModel(QWeakPointer<FlightTaskArea> area, QObject * parent) :
    QAbstractListModel(parent), _area(area)
{
    QSharedPointer<FlightTaskArea> strong = area.toStrongRef();
    if (strong.isNull())
        return;
}

int FlightTaskAreaListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    QSharedPointer<FlightTaskArea> strong = _area.toStrongRef();
    if (strong.isNull())
        return 0;

    return strong->numTasks();
}

QVariant FlightTaskAreaListModel::data(const QModelIndex &index, int role) const
{
    QSharedPointer<FlightTaskArea> strong = _area.toStrongRef();
    if (strong.isNull())
        return QVariant("Null area");

    if (!index.isValid())
        return QVariant("Invalid index");

    if (index.row() >= strong->numTasks())
        return QVariant("Row out of bounds");

    QList<QSharedPointer<FlightTask> > tasks = strong->tasks().toList();
    if (role == Qt::DisplayRole)
    {
        return QVariant(tasks[index.row()]->taskType());
    }
    else
        return QVariant();
}

Qt::ItemFlags FlightTaskAreaListModel::flags(const QModelIndex &index) const
{
    QSharedPointer<FlightTaskArea> strong = _area.toStrongRef();

    if (!index.isValid() || strong.isNull() || index.row() >= strong->numTasks())
        return Qt::NoItemFlags;
    return (Qt::ItemIsSelectable| Qt::ItemIsEnabled);
}
