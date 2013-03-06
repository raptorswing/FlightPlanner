#include "FlightTaskAreaObjectEditWidget.h"
#include "ui_FlightTaskAreaObjectEditWidget.h"

#include "FlightTaskAreaListModel.h"
#include "FlightTasks/FlyThroughTask.h"
#include "FlightTasks/NoFlyFlightTask.h"
#include "FlightTasks/CoverageTask.h"
#include "FlightTasks/SamplingTask.h"
#include "FlightTaskDelegate.h"

FlightTaskAreaObjectEditWidget::FlightTaskAreaObjectEditWidget(QWeakPointer<PlanningProblem> problem,
                                                               QPointer<FlightTaskAreaMapObject> flightTaskAreaObj,
                                                               QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FlightTaskAreaObjectEditWidget),
    _problem(problem),
    _flightTaskAreaMapObj(flightTaskAreaObj)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    if (flightTaskAreaObj.isNull())
        return;

    connect(flightTaskAreaObj->flightTaskArea().data(),
            SIGNAL(destroyed()),
            this,
            SLOT(deleteLater()));
    connect(flightTaskAreaObj->flightTaskArea().data(),
            SIGNAL(flightTaskAreaChanged()),
            this,
            SLOT(persistentEditorHack()));

    connect(flightTaskAreaObj->flightTaskArea().data(),
            SIGNAL(flightTaskAreaNameChanged()),
            this,
            SLOT(handleNameChanged()));

    FlightTaskAreaListModel * model = new FlightTaskAreaListModel(flightTaskAreaObj->flightTaskArea(),
                                                                  this->ui->taskListView);
    this->ui->taskListView->setModel(model);
    FlightTaskDelegate * delegate = new FlightTaskDelegate(_problem,
                                                           flightTaskAreaObj->flightTaskArea(),
                                                           this->ui->taskListView);
    this->ui->taskListView->setItemDelegate(delegate);

    //Initialize things
    this->handleNameChanged();
    this->persistentEditorHack();
}

FlightTaskAreaObjectEditWidget::~FlightTaskAreaObjectEditWidget()
{
    delete ui;
}

//private slot
void FlightTaskAreaObjectEditWidget::persistentEditorHack()
{
    QAbstractItemModel * model = this->ui->taskListView->model();

    for (int i = 0; i < model->rowCount(); i++)
    {
        QModelIndex index = model->index(i, 0);
        this->ui->taskListView->openPersistentEditor(index);
    }
}

//private slot
void FlightTaskAreaObjectEditWidget::handleNameChanged()
{
    if (_flightTaskAreaMapObj.isNull())
        return;

    QSharedPointer<FlightTaskArea> flightTaskArea = _flightTaskAreaMapObj->flightTaskArea().toStrongRef();
    if (flightTaskArea.isNull())
        return;

    this->ui->areaNameEdit->setText(flightTaskArea->areaName());
}


//private slot
void FlightTaskAreaObjectEditWidget::on_AddFlyThruButton_clicked()
{
    if (_flightTaskAreaMapObj.isNull())
        return;

    QSharedPointer<FlightTaskArea> flightTaskArea = _flightTaskAreaMapObj->flightTaskArea().toStrongRef();
    if (flightTaskArea.isNull())
        return;

    QSharedPointer<FlyThroughTask> task(new FlyThroughTask());
    flightTaskArea->addTask(task);
}

//private slot
void FlightTaskAreaObjectEditWidget::on_addNoFlyButton_clicked()
{
    if (_flightTaskAreaMapObj.isNull())
        return;

    QSharedPointer<FlightTaskArea> flightTaskArea = _flightTaskAreaMapObj->flightTaskArea().toStrongRef();
    if (flightTaskArea.isNull())
        return;

    QSharedPointer<NoFlyFlightTask> task(new NoFlyFlightTask());
    flightTaskArea->addTask(task);
}

//private slot
void FlightTaskAreaObjectEditWidget::on_coverageButton_clicked()
{
    if (_flightTaskAreaMapObj.isNull())
        return;

    QSharedPointer<FlightTaskArea> flightTaskArea = _flightTaskAreaMapObj->flightTaskArea().toStrongRef();
    if (flightTaskArea.isNull())
        return;

    QSharedPointer<CoverageTask> task(new CoverageTask());
    flightTaskArea->addTask(task);
}

//private slot
void FlightTaskAreaObjectEditWidget::on_addSamplingButton_clicked()
{
    if (_flightTaskAreaMapObj.isNull())
        return;

    QSharedPointer<FlightTaskArea> flightTaskArea = _flightTaskAreaMapObj->flightTaskArea().toStrongRef();
    if (flightTaskArea.isNull())
        return;

    QSharedPointer<SamplingTask> task(new SamplingTask(30.0));
    flightTaskArea->addTask(task);
}

//private slot
void FlightTaskAreaObjectEditWidget::on_areaNameEdit_textEdited(const QString &arg1)
{
    if (_flightTaskAreaMapObj.isNull())
        return;

    QSharedPointer<FlightTaskArea> flightTaskArea = _flightTaskAreaMapObj->flightTaskArea().toStrongRef();
    if (flightTaskArea.isNull())
        return;

    flightTaskArea->setAreaName(arg1);
}
