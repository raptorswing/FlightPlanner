#include "FlightTaskDelegate.h"

#include <QPainter>
#include <QPushButton>

#include "FlightTaskRowEditor.h"

FlightTaskDelegate::FlightTaskDelegate(QWeakPointer<PlanningProblem> problem,
                                       QWeakPointer<FlightTaskArea> area,
                                       QObject *parent) :
    QStyledItemDelegate(parent), _problem(problem), _area(area)
{
}

//virtual from QStyledItemDelegate
void FlightTaskDelegate::paint(QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    QSharedPointer<FlightTaskArea> area = _area.toStrongRef();
    if (!area)
        return;
    painter->save();

    const QSharedPointer<FlightTask>& task = area->tasks().at(index.row());

    QBrush backgroundBrush = option.palette.base();
    if (option.state & QStyle::State_Selected)
        backgroundBrush = option.palette.highlight();
    painter->fillRect(option.rect, backgroundBrush);

    painter->drawText(option.rect, Qt::AlignVCenter | Qt::AlignLeft, task->taskType() + " - " + task->taskName());

    painter->restore();
}

//virtual from QStyledItemDelegate
QSize FlightTaskDelegate::sizeHint(const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    return QSize(200,20);
}

//virtual from QStyledItemDelegate
QWidget *FlightTaskDelegate::createEditor(QWidget *parent,
                                          const QStyleOptionViewItem &option,
                                          const QModelIndex &index) const
{
    Q_UNUSED(option)
    QSharedPointer<FlightTaskArea> area = _area.toStrongRef();
    if (!area)
        return 0;

    return new FlightTaskRowEditor(_problem, _area, area->tasks().at(index.row()), parent);
}
