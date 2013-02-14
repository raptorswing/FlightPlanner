#include "FlightTaskAreaListModel.h"

FlightTaskAreaListModel::FlightTaskAreaListModel(QWeakPointer<FlightTaskArea> area, QObject * parent) :
    QAbstractListModel(parent), _area(area)
{
    QSharedPointer<FlightTaskArea> strong = area.toStrongRef();
    if (strong.isNull())
        return;

    connect(strong.data(),
            SIGNAL(taskAboutToAdd()),
            this,
            SLOT(handleRowsAboutToAdd()));
    connect(strong.data(),
            SIGNAL(taskAdded(QSharedPointer<FlightTask>)),
            this,
            SLOT(handleRowsAdded()));
    connect(strong.data(),
            SIGNAL(taskAboutToRemove(int)),
            this,
            SLOT(handleRowAboutToRemove(int)));
    connect(strong.data(),
            SIGNAL(taskRemoved(QSharedPointer<FlightTask>)),
            this,
            SLOT(handleRowRemoved()));
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

    const QList<QSharedPointer<FlightTask> >& tasks = strong->tasks();
    if (role == Qt::DisplayRole)
    {
        const QSharedPointer<FlightTask>& task = tasks.at(index.row());
        return QVariant(task->taskType() + " - " + task->taskName());
    }
    else
        return QVariant();
}

Qt::ItemFlags FlightTaskAreaListModel::flags(const QModelIndex &index) const
{
    QSharedPointer<FlightTaskArea> strong = _area.toStrongRef();

    if (!index.isValid() || strong.isNull() || index.row() >= strong->numTasks())
        return Qt::NoItemFlags;
    return (Qt::ItemIsSelectable| Qt::ItemIsEnabled | Qt::ItemIsEditable);
}

//public slot
void FlightTaskAreaListModel::handleRowsAboutToAdd()
{
    const int count = this->rowCount();
    this->beginInsertRows(QModelIndex(), count, count);
}

//public slot
void FlightTaskAreaListModel::handleRowsAdded()
{
    this->endInsertRows();
}

//public slot
void FlightTaskAreaListModel::handleRowAboutToRemove(int index)
{
    this->beginRemoveRows(QModelIndex(), index, index);
}

//public slot
void FlightTaskAreaListModel::handleRowRemoved()
{
    this->endRemoveRows();
}
