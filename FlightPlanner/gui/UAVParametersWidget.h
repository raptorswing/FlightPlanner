#ifndef UAVPARAMETERSWIDGET_H
#define UAVPARAMETERSWIDGET_H

#include "UAVParameters.h"
#include "PlanningProblem.h"

#include <QWidget>
#include <QSharedPointer>

namespace Ui {
class UAVParametersWidget;
}

class UAVParametersWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UAVParametersWidget(const QSharedPointer<PlanningProblem>& problem,
                                 QWidget *parent = 0);
    ~UAVParametersWidget();

    void setParameters(const UAVParameters& params);
    UAVParameters parameters() const;
    
signals:
    
public slots:

private slots:
    void on_cancelButton_clicked();

    void on_okButton_clicked();

private:
    Ui::UAVParametersWidget *ui;

    QWeakPointer<PlanningProblem> _problem;
    
};

#endif // UAVPARAMETERSWIDGET_H
