#ifndef SAMPLINGTASKEDITOR_H
#define SAMPLINGTASKEDITOR_H

#include <QWidget>
#include <QSharedPointer>

#include "FlightTasks/SamplingTask.h"

namespace Ui {
class SamplingTaskEditor;
}

class SamplingTaskEditor : public QWidget
{
    Q_OBJECT
    
public:
    explicit SamplingTaskEditor(const QSharedPointer<SamplingTask> task, QWidget *parent = 0);
    ~SamplingTaskEditor();
    
private slots:
    void saveState();
    void loadState();
    void on_cancelButton_clicked();

    void on_okButton_clicked();

private:
    Ui::SamplingTaskEditor *ui;

    QWeakPointer<SamplingTask> _task;
};

#endif // SAMPLINGTASKEDITOR_H
