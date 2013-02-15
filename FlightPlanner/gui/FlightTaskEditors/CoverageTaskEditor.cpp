#include "CoverageTaskEditor.h"

#include <QTimer>

CoverageTaskEditor::CoverageTaskEditor(const QSharedPointer<CoverageTask> task, QWidget *parent) :
    FlightTaskEditor(task, parent),
    _task(task)
{
    _myWidgets = new CoverageTaskEditorWidgets(this);
    this->addEditorWidget(_myWidgets);

    this->setWindowTitle("Edit Coverage Task");
}

CoverageTaskEditor::~CoverageTaskEditor()
{
}

//protected slot
void CoverageTaskEditor::loadSub()
{
    QSharedPointer<CoverageTask> task = _task.toStrongRef();
    if (task.isNull())
        return;

    _myWidgets->setGranularity(task->granularity());
    _myWidgets->setMaxDistance(task->maxDistance());
}

//protected slot
void CoverageTaskEditor::saveSub()
{
    QSharedPointer<CoverageTask> task = _task.toStrongRef();
    if (task.isNull())
        return;

    task->setGranularity(_myWidgets->granularity());
    task->setMaxDistance(_myWidgets->maxDistance());
}
