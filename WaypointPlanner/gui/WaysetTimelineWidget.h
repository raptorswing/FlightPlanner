#ifndef WAYSETTIMELINEWIDGET_H
#define WAYSETTIMELINEWIDGET_H

#include <QWidget>

#include "Wayset.h"
#include "UAVParameters.h"

class WaysetTimelineWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WaysetTimelineWidget(QWidget *parent = 0);

    const Wayset& currentWayset() const;
    const UAVParameters& uavParams() const;

    //virtual from QWidget
    virtual QSize sizeHint() const;

protected:
    //virtual from QWidget
    virtual void paintEvent(QPaintEvent *);
    
signals:
    
public slots:
    void setWayset(const Wayset& wayset);
    void setUAVParameters(const UAVParameters& uavParams);
    void setSelectedIndices(const QSet<int>& indices);


private:
    Wayset _wayset;
    UAVParameters _uavParams;
    QSet<int> _selectedIndices;
    
};

#endif // WAYSETTIMELINEWIDGET_H
