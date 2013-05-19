#include "DirectionalConstraintEditor.h"
#include "ui_DirectionalConstraintEditor.h"

DirectionalConstraintEditor::DirectionalConstraintEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DirectionalConstraintEditor)
{
    ui->setupUi(this);
}

DirectionalConstraintEditor::~DirectionalConstraintEditor()
{
    delete ui;
}

void DirectionalConstraintEditor::setRange(const AngleRange &range)
{
    this->ui->centerSpinBox->setValue(range.center().degrees());
    this->ui->rangeSpinBox->setValue(range.rangeDegrees());
}

AngleRange DirectionalConstraintEditor::range() const
{
    AngleRange toRet(UAVOrientation(this->ui->centerSpinBox->value(), false),
                     this->ui->rangeSpinBox->value(),
                     false);

    return toRet;
}
