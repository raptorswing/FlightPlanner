#include "SamplingTaskEditor.h"

SamplingTaskEditor::SamplingTaskEditor(QSharedPointer<PlanningProblem> problem, QSharedPointer<SamplingTask> task, QWidget *parent) :
    FlightTaskEditor(problem, task, parent),
    _task(task)
{
    _myWidgets = new SamplingTaskEditorWidgets(this);
    this->addEditorWidget(_myWidgets);

    this->setWindowTitle("Edit Sampling Task");
}

//protected slot
void SamplingTaskEditor::loadSub()
{
    QSharedPointer<SamplingTask> strong = _task.toStrongRef();
    if (strong.isNull())
        return;

    _myWidgets->setMinimumSamplingTime(strong->timeRequired());
}

//protected slot
void SamplingTaskEditor::saveSub()
{
    QSharedPointer<SamplingTask> strong = _task.toStrongRef();
    if (strong.isNull())
        return;

    strong->setTimeRequired(_myWidgets->minimumSamplingTime());
}
