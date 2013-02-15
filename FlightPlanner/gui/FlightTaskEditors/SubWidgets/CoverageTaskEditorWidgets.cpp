#include "CoverageTaskEditorWidgets.h"
#include "ui_CoverageTaskEditorWidgets.h"

CoverageTaskEditorWidgets::CoverageTaskEditorWidgets(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CoverageTaskEditorWidgets)
{
    ui->setupUi(this);
}

CoverageTaskEditorWidgets::~CoverageTaskEditorWidgets()
{
    delete ui;
}

qreal CoverageTaskEditorWidgets::granularity() const
{
    return this->ui->granularitySpinbox->value();
}

qreal CoverageTaskEditorWidgets::maxDistance() const
{
    return this->ui->maxDistanceSpinbox->value();
}

void CoverageTaskEditorWidgets::setGranularity(qreal nGran)
{
    this->ui->granularitySpinbox->setValue(nGran);
}

void CoverageTaskEditorWidgets::setMaxDistance(qreal nDist)
{
    this->ui->maxDistanceSpinbox->setValue(nDist);
}
