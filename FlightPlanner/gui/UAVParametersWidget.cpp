#include "UAVParametersWidget.h"
#include "ui_UAVParametersWidget.h"

UAVParametersWidget::UAVParametersWidget(const QSharedPointer<PlanningProblem> &problem,
                                         QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UAVParametersWidget),
    _problem(problem)
{
    ui->setupUi(this);

    if (problem.isNull())
        return;

    connect(problem.data(),
            SIGNAL(destroyed()),
            this,
            SLOT(deleteLater()));
}

UAVParametersWidget::~UAVParametersWidget()
{
}

void UAVParametersWidget::setParameters(const UAVParameters &params)
{
    this->ui->airspeedSpinbox->setValue(params.airspeed());
    this->ui->turnRadiusSpinbox->setValue(params.minTurningRadius());
    this->ui->waypointIntervalSpinbox->setValue(params.waypointInterval());
}

UAVParameters UAVParametersWidget::parameters() const
{
    return UAVParameters(this->ui->airspeedSpinbox->value(),
                         this->ui->turnRadiusSpinbox->value(),
                         this->ui->waypointIntervalSpinbox->value());
}

//private slot
void UAVParametersWidget::on_cancelButton_clicked()
{
    this->deleteLater();
}

//private slot
void UAVParametersWidget::on_okButton_clicked()
{
    QSharedPointer<PlanningProblem> problem = _problem.toStrongRef();
    if (problem.isNull())
        return;

    problem->setUAVParameters(this->parameters());
    this->deleteLater();
}
