#ifndef DEPENDENCYROW_H
#define DEPENDENCYROW_H

#include <QWidget>
#include <QWeakPointer>

#include "FlightTasks/FlightTask.h"

namespace Ui {
class DependencyRow;
}

class DependencyRow : public QWidget
{
    Q_OBJECT
    
public:
    explicit DependencyRow(QWeakPointer<FlightTask> task, QWidget *parent = 0);
    ~DependencyRow();

    const QWeakPointer<FlightTask>& task() const;

signals:
    void removeRequested(DependencyRow * toRemove);

public slots:
    void setDependencyLabel(const QString& nText);
    
private slots:
    void on_removeButton_clicked();

private:
    Ui::DependencyRow *ui;

    QWeakPointer<FlightTask> _task;
};

#endif // DEPENDENCYROW_H
