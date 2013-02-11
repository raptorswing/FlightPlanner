#include "SamplingTaskEditor.h"
#include "ui_SamplingTaskEditor.h"

SamplingTaskEditor::SamplingTaskEditor(const QSharedPointer<SamplingTask> task, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SamplingTaskEditor),
    _task(task)
{
    ui->setupUi(this);

    if (_task.isNull())
    {
        this->deleteLater();
        return;
    }

    connect(task.data(),
            SIGNAL(destroyed()),
            this,
            SLOT(deleteLater()));

    this->loadState();
}

SamplingTaskEditor::~SamplingTaskEditor()
{
    delete ui;
}

//private slot
void SamplingTaskEditor::saveState()
{
    QSharedPointer<SamplingTask> task = _task.toStrongRef();
    if (task.isNull())
        return;

    task->setTimeRequired(this->ui->samplingTimeSpinbox->value());
    task->setTimingConstraints(this->ui->timingConstraintEditor->timingConstraints());
}

//private slot
void SamplingTaskEditor::loadState()
{
    QSharedPointer<SamplingTask> task = _task.toStrongRef();
    if (task.isNull())
        return;

    this->ui->samplingTimeSpinbox->setValue(task->timeRequired());
    this->ui->timingConstraintEditor->setTimingConstraints(task->timingConstraints());
}

//private slot
void SamplingTaskEditor::on_cancelButton_clicked()
{
    this->deleteLater();
}

//private slot
void SamplingTaskEditor::on_okButton_clicked()
{
    this->saveState();
    this->deleteLater();
}
