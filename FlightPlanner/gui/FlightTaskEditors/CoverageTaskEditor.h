#ifndef COVERAGETASKEDITOR_H
#define COVERAGETASKEDITOR_H

#include <QWidget>
#include <QSharedPointer>

#include "FlightTasks/CoverageTask.h"
#include "FlightTaskEditor.h"

#include "SubWidgets/CoverageTaskEditorWidgets.h"

class CoverageTaskEditor : public FlightTaskEditor
{
    Q_OBJECT
    
public:
    explicit CoverageTaskEditor(const QSharedPointer<CoverageTask> task, QWidget *parent = 0);
    ~CoverageTaskEditor();

protected slots:
    void loadSub();
    void saveSub();

private:
    QWeakPointer<CoverageTask> _task;

    CoverageTaskEditorWidgets * _myWidgets;
};

#endif // COVERAGETASKEDITOR_H
