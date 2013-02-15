#ifndef FlightTaskEditorFactory_H
#define FlightTaskEditorFactory_H

#include <QWidget>
#include <QSharedPointer>

#include "FlightTasks/FlightTask.h"
#include "PlanningProblem.h"

class FlightTaskEditorFactory
{
public:
    static QWidget * getEditor(const QSharedPointer<PlanningProblem> problem,
                               const QSharedPointer<FlightTask>& task);

private:
    FlightTaskEditorFactory();
};

#endif // FlightTaskEditorFactory_H
