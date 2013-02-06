#include "TimingConstraintSliders.h"
#include "ui_TimingConstraintSliders.h"

TimingConstraintSliders::TimingConstraintSliders(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimingConstraintSliders)
{
    ui->setupUi(this);
}

TimingConstraintSliders::TimingConstraintSliders(int startTime, int endTime, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimingConstraintSliders)
{
    ui->setupUi(this);
    this->ui->startSlider->setValue(startTime);
    this->ui->startSpinbox->setValue(startTime);
    this->ui->endSlider->setValue(endTime);
    this->ui->endSpinbox->setValue(endTime);
}

TimingConstraintSliders::~TimingConstraintSliders()
{
    delete ui;
}

int TimingConstraintSliders::startTime() const
{
    return this->ui->startSlider->value();
}

int TimingConstraintSliders::endTime() const
{
    return this->ui->endSlider->value();
}

void TimingConstraintSliders::setStartTime(int nStart)
{
    this->ui->startSlider->setValue(nStart);
}

void TimingConstraintSliders::setEndTime(int nEnd)
{
    this->ui->endSlider->setValue(nEnd);
}

//private slot
void TimingConstraintSliders::on_startSlider_sliderMoved(int position)
{
    const int endSliderVal = this->ui->endSlider->value();
    if (endSliderVal < position)
        this->ui->endSlider->setValue(position);
}

//private slot
void TimingConstraintSliders::on_endSlider_sliderMoved(int position)
{
    const int startSliderVal = this->ui->startSlider->value();
    if (startSliderVal > position)
        this->ui->startSlider->setValue(position);
}

//private slot
void TimingConstraintSliders::on_removeButton_clicked()
{
    this->removeRequested(this);
}
