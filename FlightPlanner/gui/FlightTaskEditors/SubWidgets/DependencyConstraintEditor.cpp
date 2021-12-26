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

QList<QWeakPointer<FlightTask> > DependencyConstraintEditor::dependencies() const
{
    QList<QWeakPointer<FlightTask> > toRet;

    foreach(DependencyRow * row, _rows)
    {
        if (!row->task().isNull())
            toRet.append(row->task());
    }

    return toRet;
}

void DependencyConstraintEditor::setDependencies(const QList<QWeakPointer<FlightTask> > &nDeps)
{
    foreach(DependencyRow * row, _rows)
        row->deleteLater();
    _rows.clear();

    foreach(const QWeakPointer<FlightTask>& task, nDeps)
    {
        DependencyRow * row = new DependencyRow(task, this);
        connect(row,
                SIGNAL(removeRequested(DependencyRow*)),
                this,
                SLOT(handleConstraintWantsToRemove(DependencyRow*)));
        this->ui->rowLayout->addWidget(row);
        _rows.append(row);
    }

    this->buildMenu();
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
    _menuHelper.clear();

    QList<QWeakPointer<FlightTask> > alreadyAdded = this->dependencies();

    bool atleastOne = false;
    foreach(const QSharedPointer<FlightTaskArea>& area, problem->areas())
    {
        foreach (const QSharedPointer<FlightTask>& task, area->tasks())
        {
            if (task == ourTask)
                continue;
            else if (alreadyAdded.contains(task))
                continue;
            else if (task->taskType() == "No-Fly Zone")
                continue;
            atleastOne = true;
            QAction * action = _menu.addAction(task->taskType() + " - " + task->taskName(),
                                               this,
                                               SLOT(handleConstraintWantsToAdd()));
            _menuHelper.insert(action, task);
        }
    }

    if (!atleastOne)
        _menu.addAction("Can't add dependency - no other tasks")->setDisabled(true);

    this->ui->addDependencyButton->setMenu(&_menu);
}

//private slot
void DependencyConstraintEditor::handleConstraintWantsToAdd()
{
    QObject * sender = QObject::sender();
    if (!_menuHelper.contains(sender))
        return;

    QWeakPointer<FlightTask> toAdd = _menuHelper.value(sender);

    DependencyRow * row = new DependencyRow(toAdd, this);
    _rows.append(row);
    this->ui->rowLayout->addWidget(row);

    connect(row,
            SIGNAL(removeRequested(DependencyRow*)),
            this,
            SLOT(handleConstraintWantsToRemove(DependencyRow*)));

    this->buildMenu();
}

//private slot
void DependencyConstraintEditor::handleConstraintWantsToRemove(DependencyRow *row)
{
    _rows.removeOne(row);
    row->deleteLater();

    this->buildMenu();
}
