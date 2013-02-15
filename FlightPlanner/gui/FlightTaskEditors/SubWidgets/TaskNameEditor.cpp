#include "TaskNameEditor.h"
#include "ui_TaskNameEditor.h"

TaskNameEditor::TaskNameEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TaskNameEditor)
{
    ui->setupUi(this);
}

TaskNameEditor::~TaskNameEditor()
{
    delete ui;
}

QString TaskNameEditor::name() const
{
    return this->ui->taskNameEdit->text();
}

void TaskNameEditor::setName(const QString &nName)
{
    this->ui->taskNameEdit->setText(nName);
}
