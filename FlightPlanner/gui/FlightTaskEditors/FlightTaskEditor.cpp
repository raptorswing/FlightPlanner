#include "FlightTaskEditor.h"
#include "ui_FlightTaskEditor.h"

#include <QLayout>
#include <QVBoxLayout>
#include <QTimer>

FlightTaskEditor::FlightTaskEditor(QSharedPointer<PlanningProblem> problem,
                                   QSharedPointer<FlightTask> task, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FlightTaskEditor),
    _baseTask(task),
    _problem(problem)
{
    ui->setupUi(this);

    if (_baseTask.isNull())
    {
        this->deleteLater();
        return;
    }

    connect(_baseTask.data(),
            SIGNAL(destroyed()),
            this,
            SLOT(deleteLater()));

    //Virtual methods do not work at this point (construction) so we schedule them for later.
    QTimer::singleShot(1, this, SLOT(load()));

    _taskNameEditor = new TaskNameEditor(this);
    this->addEditorWidget(_taskNameEditor);

    //These two editors only for coverage or sampling
    if (task->taskType() == "Coverage"
            || task->taskType() == "Sampling")
    {
        _sensorTypeEditor = new SensorTypeEditor(this);
        this->addEditorWidget(_sensorTypeEditor);

        _directionalConstraintEditor = new DirectionalConstraintEditor(this);
        this->addEditorWidget(_directionalConstraintEditor);

        _distanceConstraintEditor = new MinSensingDistEditor(this);
        this->addEditorWidget(_distanceConstraintEditor);
    }

    _timingConstraintsEditor = new TimingConstraintEditor(this);
    this->addEditorWidget(_timingConstraintsEditor);

    _dependencyConstraintsEditor = new DependencyConstraintEditor(_problem, _baseTask, this);
    this->addEditorWidget(_dependencyConstraintsEditor);
}

FlightTaskEditor::~FlightTaskEditor()
{
    delete ui;
}

//protected
void FlightTaskEditor::addEditorWidget(QWidget *widget)
{
    QVBoxLayout * layout = this->ui->fancyLayout;
    if (layout == 0)
        return;

    layout->addWidget(widget);
    widget->setParent(this);
}

//public slot
void FlightTaskEditor::load()
{
    QSharedPointer<FlightTask> strong = _baseTask.toStrongRef();
    if (strong.isNull())
        return;

    _taskNameEditor->setName(strong->taskName());

    if (strong->taskType() == "Coverage"
            || strong->taskType() == "Sampling")
    {
        _sensorTypeEditor->setSensorType(strong->sensorType());
        _directionalConstraintEditor->setRange(strong->validSensorAngleRange());
        _distanceConstraintEditor->setRange(strong->minSensingDistance(), strong->maxSensingDistance());
    }
    _timingConstraintsEditor->setTimingConstraints(strong->timingConstraints());
    _dependencyConstraintsEditor->setDependencies(strong->dependencyConstraints());
    this->loadSub();
}

//public slot
void FlightTaskEditor::save()
{
    QSharedPointer<FlightTask> strong = _baseTask.toStrongRef();
    if (strong.isNull())
        return;

    strong->setTaskName(_taskNameEditor->name());

    if (strong->taskType() == "Coverage"
            || strong->taskType() == "Sampling")
    {
        strong->setSensorType(_sensorTypeEditor->sensorType());
        strong->setValidSensorAngleRange(_directionalConstraintEditor->range());
        strong->setSensingRange(_distanceConstraintEditor->min(), _distanceConstraintEditor->max());
    }

    strong->setTimingConstraints(_timingConstraintsEditor->timingConstraints());
    strong->setDependencyConstraints(_dependencyConstraintsEditor->dependencies());

    this->saveSub();
}

//private slot
void FlightTaskEditor::on_cancelButton_clicked()
{
    this->deleteLater();
}

//private slot
void FlightTaskEditor::on_okButton_clicked()
{
    this->save();
    this->deleteLater();
}
