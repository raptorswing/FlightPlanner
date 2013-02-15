#include "SamplingTaskEditorWidgets.h"
#include "ui_SamplingTaskEditorWidgets.h"

SamplingTaskEditorWidgets::SamplingTaskEditorWidgets(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::SamplingTaskEditorWidgets)
{
    ui->setupUi(this);
}

SamplingTaskEditorWidgets::~SamplingTaskEditorWidgets()
{
    delete ui;
}

int SamplingTaskEditorWidgets::minimumSamplingTime() const
{
    return this->ui->minSamplingSpinbox->value();
}

//public slot
void SamplingTaskEditorWidgets::setMinimumSamplingTime(int nTime)
{
    this->ui->minSamplingSpinbox->setValue(nTime);
}
