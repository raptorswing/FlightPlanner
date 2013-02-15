#include "DependencyConstraintEditor.h"
#include "ui_DependencyConstraintEditor.h"

#include <QtDebug>

DependencyConstraintEditor::DependencyConstraintEditor(QWeakPointer<PlanningProblem> problem, QWeakPointer<FlightTask> task,
                                                       QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::DependencyConstraintEditor),
    _problem(problem),
    _task(task)
{
    ui->setupUi(this);

    QSharedPointer<PlanningProblem> strong = _problem.toStrongRef();
    if (strong.isNull())
        return;

    connect(strong.data(),
            SIGNAL(planningProblemChanged()),
            this,
            SLOT(buildMenu()));

    this->buildMenu();
}

DependencyConstraintEditor::~DependencyConstraintEditor()
{
    delete ui;
}

//private slot
void DependencyConstraintEditor::buildMenu()
{
    QSharedPointer<PlanningProblem> problem = _problem.toStrongRef();
    if (problem.isNull())
        return;

    QSharedPointer<FlightTask> ourTask = _task.toStrongRef();
    if (ourTask.isNull())
        return;

    _menu.clear();

    bool atleastOne = false;
    foreach(const QSharedPointer<FlightTaskArea>& area, problem->areas())
    {
        foreach (const QSharedPointer<FlightTask>& task, area->tasks())
        {
            if (task == ourTask)
                continue;
            atleastOne = true;
            _menu.addAction(task->taskType() + " - " + task->taskName());
        }
    }

    if (!atleastOne)
        _menu.addAction("Can't add dependency - no other tasks")->setDisabled(true);

    this->ui->addDependencyButton->setMenu(&_menu);
}
