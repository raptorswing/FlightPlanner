#include "FlyThroughTaskEditor.h"

FlyThroughTaskEditor::FlyThroughTaskEditor(QSharedPointer<PlanningProblem> problem,
                                           QSharedPointer<FlyThroughTask> task,
                                           QWidget *parent) :
    FlightTaskEditor(problem, task, parent), _task(task)
{
}

//virtual protected slot
void FlyThroughTaskEditor::loadSub()
{
}

//virtual protected slot
void FlyThroughTaskEditor::saveSub()
{
}
