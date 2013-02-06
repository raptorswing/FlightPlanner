#ifndef FlightTaskEditorFactory_H
#define FlightTaskEditorFactory_H

#include <QWidget>
#include <QSharedPointer>

#include "FlightTasks/FlightTask.h"

class FlightTaskEditorFactory
{
public:
    static QWidget * getEditor(const QSharedPointer<FlightTask>& task);

private:
    FlightTaskEditorFactory();
};

#endif // FlightTaskEditorFactory_H
