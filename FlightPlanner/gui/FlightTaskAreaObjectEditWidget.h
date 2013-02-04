#ifndef FLIGHTTASKAREAOBJECTEDITWIDGET_H
#define FLIGHTTASKAREAOBJECTEDITWIDGET_H

#include <QWidget>
#include <QPointer>

#include "MapObjects/FlightTaskAreaMapObject.h"

namespace Ui {
class FlightTaskAreaObjectEditWidget;
}

class FlightTaskAreaObjectEditWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit FlightTaskAreaObjectEditWidget(QPointer<FlightTaskAreaMapObject> taskAreaObj = 0,
                                            QWidget *parent = 0);
    ~FlightTaskAreaObjectEditWidget();
    
private slots:

    void on_AddFlyThruButton_clicked();

    void on_addNoFlyButton_clicked();

    void on_coverageButton_clicked();

private:
    Ui::FlightTaskAreaObjectEditWidget *ui;

    QPointer<FlightTaskAreaMapObject> _flightTaskAreaMapObj;
};

#endif // FLIGHTTASKAREAOBJECTEDITWIDGET_H
