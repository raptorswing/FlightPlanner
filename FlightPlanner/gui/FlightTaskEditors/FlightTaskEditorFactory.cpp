#include "FlightTaskEditorFactory.h"

#include "CoverageTaskEditor.h"
#include "SamplingTaskEditor.h"
#include "FlyThroughTaskEditor.h"

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
    else if (type == "Fly Through")
    {
        QSharedPointer<FlyThroughTask> flyThrough = task.objectCast<FlyThroughTask>();
        toRet = new FlyThroughTaskEditor(problem, flyThrough);
    }

    toRet->setAttribute(Qt::WA_DeleteOnClose);
    return toRet;
}


//private
FlightTaskEditorFactory::FlightTaskEditorFactory()
{
}
