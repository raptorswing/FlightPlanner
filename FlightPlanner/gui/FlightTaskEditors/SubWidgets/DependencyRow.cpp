#include "DependencyRow.h"
#include "ui_DependencyRow.h"

DependencyRow::DependencyRow(QWeakPointer<FlightTask> task, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DependencyRow),
    _task(task)
{
    ui->setupUi(this);

    QSharedPointer<FlightTask> strong = _task.toStrongRef();
    if (strong.isNull())
        return;

    this->setDependencyLabel(strong->taskType() + " - " + strong->taskName());
}

DependencyRow::~DependencyRow()
{
    delete ui;
}

const QWeakPointer<FlightTask> &DependencyRow::task() const
{
    return _task;
}

//public slot
void DependencyRow::setDependencyLabel(const QString &nText)
{
    this->ui->displayLabel->setText(nText);
}

//private slot
void DependencyRow::on_removeButton_clicked()
{
    this->removeRequested(this);
}
