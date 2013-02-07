#include "UAVParametersWidget.h"
#include "ui_UAVParametersWidget.h"

UAVParametersWidget::UAVParametersWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UAVParametersWidget)
{
    ui->setupUi(this);
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
