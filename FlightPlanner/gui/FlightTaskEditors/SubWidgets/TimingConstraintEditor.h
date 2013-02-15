#ifndef TIMINGCONSTRAINTEDITOR_H
#define TIMINGCONSTRAINTEDITOR_H

#include <QWidget>
#include <QList>

#include "TimingConstraintSliders.h"
#include "FlightTasks/TimingConstraint.h"

namespace Ui {
class TimingConstraintEditor;
}

class TimingConstraintEditor : public QWidget
{
    Q_OBJECT
    
public:
    explicit TimingConstraintEditor(QWidget *parent = 0);
    ~TimingConstraintEditor();

    QList<TimingConstraint> timingConstraints() const;
    void setTimingConstraints(const QList<TimingConstraint>& nConst);

public slots:
    void addConstraintRow();
    
private slots:
    void handleRowRemoveRequested(TimingConstraintSliders * slider);
    void on_addTimingConstraintButton_2_clicked();

private:
    Ui::TimingConstraintEditor *ui;

    QList<TimingConstraintSliders *> _sliders;
};

#endif // TIMINGCONSTRAINTEDITOR_H
