#include "FlightTaskEditorFactory.h"

#include "CoverageTaskEditor.h"
#include "SamplingTaskEditor.h"

QWidget *FlightTaskEditorFactory::getEditor(const QSharedPointer<PlanningProblem> problem,
                                            const QSharedPointer<FlightTask> &task)
{
    const QString type = task->taskType();
    QWidget * toRet = new QWidget();

    if (type == "Coverage")
    {
        QSharedPointer<CoverageTask> coverageTask = task.objectCast<CoverageTask>();
        toRet = new CoverageTaskEditor(problem, coverageTask);
    }
    else if (type == "Sampling")
    {
        QSharedPointer<SamplingTask> samplingTask = task.objectCast<SamplingTask>();
        toRet = new SamplingTaskEditor(problem, samplingTask);
    }

    toRet->setAttribute(Qt::WA_DeleteOnClose);
    return toRet;
}


//private
FlightTaskEditorFactory::FlightTaskEditorFactory()
{
}
