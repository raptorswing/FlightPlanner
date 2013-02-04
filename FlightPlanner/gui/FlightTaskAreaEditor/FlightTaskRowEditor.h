#ifndef FLIGHTTASKROWEDITOR_H
#define FLIGHTTASKROWEDITOR_H

#include <QWidget>

#include "FlightTaskArea.h"
#include "FlightTasks/FlightTask.h"

namespace Ui {
class FlightTaskRowEditor;
}

class FlightTaskRowEditor : public QWidget
{
    Q_OBJECT
    
public:
    explicit FlightTaskRowEditor(QWeakPointer<FlightTaskArea> area,
                                 QWeakPointer<FlightTask> task,
                                 QWidget *parent = 0);
    ~FlightTaskRowEditor();
    
private slots:
    void on_editTaskButton_clicked();

    void on_deleteTaskButton_clicked();

private:
    Ui::FlightTaskRowEditor *ui;

    QWeakPointer<FlightTaskArea> _area;
    QWeakPointer<FlightTask> _task;
};

#endif // FLIGHTTASKROWEDITOR_H
