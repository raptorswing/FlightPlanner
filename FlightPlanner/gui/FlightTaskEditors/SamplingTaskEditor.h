#ifndef SAMPLINGTASKEDITOR_H
#define SAMPLINGTASKEDITOR_H

#include "FlightTaskEditor.h"

#include <QSharedPointer>

#include "FlightTasks/SamplingTask.h"
#include "SubWidgets/SamplingTaskEditorWidgets.h"

class SamplingTaskEditor : public FlightTaskEditor
{
    Q_OBJECT
public:
    explicit SamplingTaskEditor(QSharedPointer<PlanningProblem> problem,
                                QSharedPointer<SamplingTask> task, QWidget *parent = 0);
    
signals:
    
public slots:

protected slots:
    virtual void loadSub();
    virtual void saveSub();

private:
    QWeakPointer<SamplingTask> _task;

    SamplingTaskEditorWidgets * _myWidgets;
    
};

#endif // SAMPLINGTASKEDITOR_H
