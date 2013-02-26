#ifndef FLYTHROUGHTASKEDITOR_H
#define FLYTHROUGHTASKEDITOR_H

#include "FlightTaskEditor.h"

#include "FlightTasks/FlyThroughTask.h"

class FlyThroughTaskEditor : public FlightTaskEditor
{
    Q_OBJECT
public:
    explicit FlyThroughTaskEditor(QSharedPointer<PlanningProblem> problem,
                                  QSharedPointer<FlyThroughTask> task,
                                  QWidget *parent = 0);
    
signals:
    
public slots:

protected slots:
    virtual void loadSub();
    virtual void saveSub();

private:
    QWeakPointer<FlyThroughTask> _task;
    
};

#endif // FLYTHROUGHTASKEDITOR_H
