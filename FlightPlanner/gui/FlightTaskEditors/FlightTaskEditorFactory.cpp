#include "FlightTaskEditorFactory.h"

#include "CoverageTaskEditor.h"

QWidget *FlightTaskEditorFactory::getEditor(const QSharedPointer<FlightTask> &task)
{
    const QString type = task->taskType();
    QWidget * toRet = new QWidget();

    if (type == "Coverage")
    {
        QSharedPointer<CoverageTask> coverageTask = task.objectCast<CoverageTask>();
        toRet = new CoverageTaskEditor(coverageTask);
    }

    toRet->setAttribute(Qt::WA_DeleteOnClose);
    return toRet;
}


//private
FlightTaskEditorFactory::FlightTaskEditorFactory()
{
}
