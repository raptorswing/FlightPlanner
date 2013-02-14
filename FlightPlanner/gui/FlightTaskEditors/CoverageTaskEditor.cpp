#include "CoverageTaskEditor.h"
#include "ui_CoverageTaskEditor.h"

#include <QTimer>

CoverageTaskEditor::CoverageTaskEditor(const QSharedPointer<CoverageTask> task, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CoverageTaskEditor),
    _task(task)
{
    ui->setupUi(this);

    if (_task.isNull())
    {
        QTimer::singleShot(1, this, SLOT(deleteLater()));
        return;
    }

    connect(task.data(),
            SIGNAL(destroyed()),
            this,
            SLOT(deleteLater()));

    this->loadState();
}

CoverageTaskEditor::~CoverageTaskEditor()
{
    delete ui;
}

//private slot
void CoverageTaskEditor::loadState()
{
    QSharedPointer<CoverageTask> task = _task.toStrongRef();
    if (task.isNull())
        return;

    this->ui->granularitySpinbox->setValue(task->granularity());
    this->ui->maxDistanceSpinbox->setValue(task->maxDistance());
    this->ui->timingConstraintEditor->setTimingConstraints(task->timingConstraints());
    this->ui->taskNameEditor->setName(task->taskName());
}

//private slot
void CoverageTaskEditor::saveState()
{
    QSharedPointer<CoverageTask> task = _task.toStrongRef();
    if (task.isNull())
        return;

    task->setGranularity(this->ui->granularitySpinbox->value());
    task->setMaxDistance(this->ui->maxDistanceSpinbox->value());
    task->setTimingConstraints(this->ui->timingConstraintEditor->timingConstraints());
    task->setTaskName(this->ui->taskNameEditor->name());
}

//private slot
void CoverageTaskEditor::on_cancelButton_clicked()
{
    this->deleteLater();
}

//private slot
void CoverageTaskEditor::on_okButton_clicked()
{
    this->saveState();
    this->deleteLater();
}
