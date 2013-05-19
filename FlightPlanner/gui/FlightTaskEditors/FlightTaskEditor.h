#ifndef FLIGHTTASKEDITOR_H
#define FLIGHTTASKEDITOR_H

#include <QWidget>
#include <QSharedPointer>

#include "FlightTasks/FlightTask.h"

#include "SubWidgets/TaskNameEditor.h"
#include "SubWidgets/SensorTypeEditor.h"
#include "SubWidgets/DirectionalConstraintEditor.h"
#include "SubWidgets/MinSensingDistEditor.h"
#include "SubWidgets/TimingConstraintEditor.h"
#include "SubWidgets/DependencyConstraintEditor.h"

#include "PlanningProblem.h"

namespace Ui {
class FlightTaskEditor;
}

class FlightTaskEditor : public QWidget
{
    Q_OBJECT
    
public:
    explicit FlightTaskEditor(QSharedPointer<PlanningProblem> problem,
                              QSharedPointer<FlightTask> task, QWidget *parent = 0);
    virtual ~FlightTaskEditor();

protected:
    /**
     * @brief addEditorWidget is a method for subclasses to add their custom GUI elements to the layout.
     * Widgets will be added vertically top-to-bottom, first-come, first-served.
     * @param widget
     */
    void addEditorWidget(QWidget * widget);

public slots:
    void load();
    void save();

protected slots:
    virtual void loadSub()=0;
    virtual void saveSub()=0;
    
private slots:
    void on_cancelButton_clicked();

    void on_okButton_clicked();

private:
    Ui::FlightTaskEditor *ui;
    QWeakPointer<FlightTask> _baseTask;

    TaskNameEditor * _taskNameEditor;
    SensorTypeEditor * _sensorTypeEditor;
    DirectionalConstraintEditor * _directionalConstraintEditor;
    MinSensingDistEditor * _distanceConstraintEditor;
    TimingConstraintEditor * _timingConstraintsEditor;
    DependencyConstraintEditor * _dependencyConstraintsEditor;

    QWeakPointer<PlanningProblem> _problem;
};

#endif // FLIGHTTASKEDITOR_H
