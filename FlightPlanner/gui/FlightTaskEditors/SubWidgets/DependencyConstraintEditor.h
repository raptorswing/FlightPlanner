#ifndef DEPENDENCYCONSTRAINTEDITOR_H
#define DEPENDENCYCONSTRAINTEDITOR_H

#include <QGroupBox>
#include <QSharedPointer>
#include <QMenu>
#include <QList>
#include <QHash>

#include "PlanningProblem.h"
#include "DependencyRow.h"

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

    QList<QWeakPointer<FlightTask> > dependencies() const;

public slots:
    void setDependencies(const QList<QWeakPointer<FlightTask> >& nDeps);
    
private slots:
    void buildMenu();

    void handleConstraintWantsToAdd();
    void handleConstraintWantsToRemove(DependencyRow * row);

private:
    Ui::DependencyConstraintEditor *ui;

    QWeakPointer<PlanningProblem> _problem;
    QWeakPointer<FlightTask> _task;

    QMenu _menu;
    QHash<QObject *, QWeakPointer<FlightTask> > _menuHelper;

    QList<DependencyRow *> _rows;
};

#endif // DEPENDENCYCONSTRAINTEDITOR_H
