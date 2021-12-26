#include "TimingConstraintEditor.h"
#include "ui_TimingConstraintEditor.h"

#include <QtDebug>
#include <QMessageBox>

TimingConstraintEditor::TimingConstraintEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimingConstraintEditor)
{
    ui->setupUi(this);
}

TimingConstraintEditor::~TimingConstraintEditor()
{
    delete ui;
}

QList<TimingConstraint> TimingConstraintEditor::timingConstraints() const
{
    QList<TimingConstraint> toRet;
    foreach(TimingConstraintSliders * sliders, _sliders)
        toRet.append(TimingConstraint(sliders->startTime(), sliders->endTime()));
    return toRet;
}

void TimingConstraintEditor::setTimingConstraints(const QList<TimingConstraint> &nConst)
{
    foreach(TimingConstraintSliders * slider, _sliders)
        slider->deleteLater();
    _sliders.clear();

    foreach(const TimingConstraint& constraint, nConst)
    {
        TimingConstraintSliders * newSliders = new TimingConstraintSliders(constraint.start(),
                                                                           constraint.end(),
                                                                           this);
        connect(newSliders,
                SIGNAL(removeRequested(TimingConstraintSliders*)),
                this,
                SLOT(handleRowRemoveRequested(TimingConstraintSliders*)));
        this->ui->rowLayout->addWidget(newSliders);
        _sliders.append(newSliders);
    }
}

//public slot
void TimingConstraintEditor::addConstraintRow()
{
    TimingConstraintSliders * newSliders = new TimingConstraintSliders(this);
    connect(newSliders,
            SIGNAL(removeRequested(TimingConstraintSliders*)),
            this,
            SLOT(handleRowRemoveRequested(TimingConstraintSliders*)));

    this->ui->rowLayout->addWidget(newSliders);
    _sliders.append(newSliders);
}

//private slot
void TimingConstraintEditor::handleRowRemoveRequested(TimingConstraintSliders *slider)
{
    const int index = _sliders.indexOf(slider);
    if (index <= -1)
        return;

    if (_sliders.size() == 1)
    {
        QMessageBox::information(this, "Cannot Remove", "Sorry, you can't remove the last time window.\n\nEach task must have at least one duration of time during which it can be flown by the UAV.");
        return;
    }

    _sliders.removeAt(index);
    slider->deleteLater();
}

//private slot
void TimingConstraintEditor::on_addTimingConstraintButton_2_clicked()
{
    this->addConstraintRow();
}
