#include "FlightTaskAreaObjectEditWidget.h"
#include "ui_FlightTaskAreaObjectEditWidget.h"

#include "FlightTaskAreaListModel.h"
#include "FlightTasks/FlyThroughTask.h"
#include "FlightTasks/NoFlyFlightTask.h"
#include "FlightTasks/CoverageTask.h"
#include "FlightTaskDelegate.h"

FlightTaskAreaObjectEditWidget::FlightTaskAreaObjectEditWidget(QPointer<FlightTaskAreaMapObject> flightTaskAreaObj,
                                                               QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FlightTaskAreaObjectEditWidget),
    _flightTaskAreaMapObj(flightTaskAreaObj)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    if (flightTaskAreaObj.isNull())
        return;

    connect(flightTaskAreaObj.data(),
            SIGNAL(destroyed()),
            this,
            SLOT(deleteLater()));

    FlightTaskAreaListModel * model = new FlightTaskAreaListModel(flightTaskAreaObj->flightTaskArea(),
                                                                  this->ui->taskListView);
    this->ui->taskListView->setModel(model);
    FlightTaskDelegate * delegate = new FlightTaskDelegate(flightTaskAreaObj->flightTaskArea(),
                                                           this->ui->taskListView);
    this->ui->taskListView->setItemDelegate(delegate);
    this->ui->taskListView->setEditTriggers(QAbstractItemView::AllEditTriggers);
}

FlightTaskAreaObjectEditWidget::~FlightTaskAreaObjectEditWidget()
{
    delete ui;
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
