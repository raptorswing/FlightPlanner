#ifndef FLIGHTTASKAREAOBJECTEDITWIDGET_H
#define FLIGHTTASKAREAOBJECTEDITWIDGET_H

#include <QWidget>
#include <QPointer>
#include <QSharedPointer>

#include "MapObjects/FlightTaskAreaMapObject.h"

#include "PlanningProblem.h"

namespace Ui {
class FlightTaskAreaObjectEditWidget;
}

class FlightTaskAreaObjectEditWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit FlightTaskAreaObjectEditWidget(QWeakPointer<PlanningProblem> problem,
                                            QPointer<FlightTaskAreaMapObject> taskAreaObj = 0,
                                            QWidget *parent = 0);
    ~FlightTaskAreaObjectEditWidget();
    
private slots:
    void persistentEditorHack();

    void handleNameChanged();

    void on_AddFlyThruButton_clicked();

    void on_addNoFlyButton_clicked();

    void on_coverageButton_clicked();

    void on_addSamplingButton_clicked();

    void on_areaNameEdit_editingFinished();

private:
    Ui::FlightTaskAreaObjectEditWidget *ui;

    QWeakPointer<PlanningProblem> _problem;
    QPointer<FlightTaskAreaMapObject> _flightTaskAreaMapObj;
};

#endif // FLIGHTTASKAREAOBJECTEDITWIDGET_H
