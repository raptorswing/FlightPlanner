#ifndef COVERAGETASKEDITOR_H
#define COVERAGETASKEDITOR_H

#include <QWidget>
#include <QSharedPointer>

#include "FlightTasks/CoverageTask.h"

namespace Ui {
class CoverageTaskEditor;
}

class CoverageTaskEditor : public QWidget
{
    Q_OBJECT
    
public:
    explicit CoverageTaskEditor(const QSharedPointer<CoverageTask> task, QWidget *parent = 0);
    ~CoverageTaskEditor();

private slots:
    void loadState();
    void saveState();
    
    void on_cancelButton_clicked();

    void on_okButton_clicked();

private:
    Ui::CoverageTaskEditor *ui;

    QWeakPointer<CoverageTask> _task;
};

#endif // COVERAGETASKEDITOR_H
