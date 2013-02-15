#include "FlightTaskRowEditor.h"
#include "ui_FlightTaskRowEditor.h"

#include "gui/FlightTaskEditors/FlightTaskEditorFactory.h"

#include <QtDebug>

FlightTaskRowEditor::FlightTaskRowEditor(QWeakPointer<PlanningProblem> problem,
                                         QWeakPointer<FlightTaskArea> area,
                                         QWeakPointer<FlightTask> task,
                                         QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FlightTaskRowEditor),
    _problem(problem),
    _area(area),
    _task(task)
{
    ui->setupUi(this);

    QSharedPointer<FlightTask> strongTask = task.toStrongRef();
    if (strongTask.isNull())
        return;
    QSharedPointer<FlightTaskArea> strongArea = area.toStrongRef();
    if (strongArea.isNull())
        return;

    //Set our label to be the task's name/type
    ui->taskNameLabel->setText(strongTask->taskType());

    //If the task or the area go away we need to close
    connect(strongArea.data(),
            SIGNAL(destroyed()),
            this,
            SLOT(deleteLater()));
    connect(strongTask.data(),
            SIGNAL(destroyed()),
            this,
            SLOT(deleteLater()));
}

FlightTaskRowEditor::~FlightTaskRowEditor()
{
    delete ui;
}

void FlightTaskRowEditor::on_editTaskButton_clicked()
{
    QSharedPointer<FlightTask> task = _task.toStrongRef();
    if (task.isNull())
        return;

    QSharedPointer<PlanningProblem> problem = _problem.toStrongRef();
    if (problem.isNull())
        return;

    QWidget * widget = FlightTaskEditorFactory::getEditor(problem, task);
    widget->show();
}

void FlightTaskRowEditor::on_deleteTaskButton_clicked()
{
    QSharedPointer<FlightTaskArea> area = _area.toStrongRef();
    if (area.isNull())
        return;
    QSharedPointer<FlightTask> task = _task.toStrongRef();
    if (task.isNull())
        return;

    area->removeTask(task);
}
