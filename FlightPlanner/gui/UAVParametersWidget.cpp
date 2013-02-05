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
