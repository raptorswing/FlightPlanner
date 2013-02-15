#ifndef DEPENDENCYCONSTRAINTEDITOR_H
#define DEPENDENCYCONSTRAINTEDITOR_H

#include <QGroupBox>
#include <QSharedPointer>
#include <QMenu>

#include "PlanningProblem.h"

namespace Ui {
class DependencyConstraintEditor;
}

class DependencyConstraintEditor : public QGroupBox
{
    Q_OBJECT
    
public:
    explicit DependencyConstraintEditor(QWeakPointer<PlanningProblem> problem,
                                        QWeakPointer<FlightTask> task,
                                        QWidget *parent = 0);
    ~DependencyConstraintEditor();
    
private slots:
    void buildMenu();

private:
    Ui::DependencyConstraintEditor *ui;

    QWeakPointer<PlanningProblem> _problem;
    QWeakPointer<FlightTask> _task;

    QMenu _menu;
};

#endif // DEPENDENCYCONSTRAINTEDITOR_H
