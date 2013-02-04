#ifndef FLIGHTTASKDELEGATE_H
#define FLIGHTTASKDELEGATE_H

#include <QStyledItemDelegate>

#include "FlightTaskArea.h"

class FlightTaskDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit FlightTaskDelegate(QWeakPointer<FlightTaskArea> area,
                                QObject *parent = 0);

    //virtual from QStyledItemDelegate
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    //virtual from QStyledItemDelegate
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    //virtual from QStyledItemDelegate
    virtual QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    
signals:
    
public slots:

private:
    QWeakPointer<FlightTaskArea> _area;
    
};

#endif // FLIGHTTASKDELEGATE_H
