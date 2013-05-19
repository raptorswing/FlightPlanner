#include "MinSensingDistEditor.h"
#include "ui_MinSensingDistEditor.h"

MinSensingDistEditor::MinSensingDistEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MinSensingDistEditor)
{
    ui->setupUi(this);
}

MinSensingDistEditor::~MinSensingDistEditor()
{
    delete ui;
}

qreal MinSensingDistEditor::min() const
{
    return this->ui->minDistSpinbox->value();
}

qreal MinSensingDistEditor::max() const
{
    return this->ui->maxDistSpinbox->value();
}

void MinSensingDistEditor::setRange(qreal min, qreal max)
{
    this->ui->minDistSpinbox->setValue(min);
    this->ui->maxDistSpinbox->setValue(max);
}

//private slot
void MinSensingDistEditor::on_minDistSpinbox_valueChanged(double arg1)
{
    if (arg1 > this->ui->maxDistSpinbox->value())
        this->ui->maxDistSpinbox->setValue(arg1);
}

//private slot
void MinSensingDistEditor::on_maxDistSpinbox_valueChanged(double arg1)
{
    if (arg1 < this->ui->minDistSpinbox->value())
        this->ui->minDistSpinbox->setValue(arg1);
}
