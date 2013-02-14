#include "FlightTaskAreaMapObject.h"

#include <QtDebug>
#include <QMenu>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>

#include "gui/FlightTaskAreaEditor/FlightTaskAreaObjectEditWidget.h"
#include "FlightTasks/CoverageTask.h"

//We take the planning problem as input so that we can delete the area from within it when needed
FlightTaskAreaMapObject::FlightTaskAreaMapObject(QWeakPointer<PlanningProblem> problem,
                                                 QWeakPointer<FlightTaskArea> taskArea,
                                                 QColor fillColor) :
    PolygonObject(QPolygon(), fillColor), _prob(problem), _area(taskArea)
{
    QSharedPointer<FlightTaskArea> strong = taskArea.toStrongRef();
    if (strong.isNull())
    {
        qWarning() << this << "got null flight task area";
        this->deleteLater();
        return;
    }

    FlightTaskArea * conv = strong.data();
    connect(conv,
            SIGNAL(geoPolyChanged(QPolygonF)),
            this,
            SLOT(handleFlightAreaGeoPolyChanged(QPolygonF)));
    connect(this,
            SIGNAL(polygonChanged(QPolygonF)),
            conv,
            SLOT(setGeoPoly(QPolygonF)));

    //Use these to force initialization
    this->handleFlightAreaGeoPolyChanged(strong->geoPoly());

    this->setZValue(1.0);
}

QWeakPointer<FlightTaskArea> FlightTaskAreaMapObject::flightTaskArea() const
{
    return _area;
}

//virtual from PolygonObject
void FlightTaskAreaMapObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QSharedPointer<FlightTaskArea> area = _area.toStrongRef();
    if (area.isNull())
        return;

    if (area->tasks().isEmpty())
        PolygonObject::paint(painter, option, widget);
    else
    {
        QString taskType = "";
        foreach(const QSharedPointer<FlightTask>& task, area->tasks())
        {
            if (taskType.isEmpty())
                taskType = task->taskType();
            else if (task->taskType() != taskType)
            {
                taskType = "Mixed";
                break;
            }
        }

        QColor fillColor(255, 255, 255, 100);
        if (taskType == "No-Fly Zone")
            fillColor = QColor(255, 0, 0, 100);
        else if (taskType == "Coverage")
            fillColor = QColor(0, 255, 0, 100);
        else if (taskType == "Sampling")
            fillColor = QColor(0, 0, 255, 100);
        this->setFillColor(fillColor);
    }

    PolygonObject::paint(painter, option, widget);

}

//protected, virtual from QGraphicsItem
void FlightTaskAreaMapObject::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu(event->widget());
    menu.addAction("Edit Tasks", this, SLOT(spawnConfigurationWidget()));
    menu.addAction("Delete", this, SLOT(handleUserDeleteRequest()));
    menu.exec(event->screenPos());
}

void FlightTaskAreaMapObject::keyReleaseEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Delete))
    {
        this->handleUserDeleteRequest();
        event->accept();
    }
    else
        event->ignore();
}

void FlightTaskAreaMapObject::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    this->spawnConfigurationWidget();
}

//private slot
void FlightTaskAreaMapObject::spawnConfigurationWidget()
{
    FlightTaskAreaObjectEditWidget * editor = new FlightTaskAreaObjectEditWidget(this);
    editor->setAttribute(Qt::WA_DeleteOnClose);
    editor->show();
}

//private slot
void FlightTaskAreaMapObject::handleFlightAreaGeoPolyChanged(const QPolygonF &geoPoly)
{
    this->setGeoPoly(geoPoly);
}

//private slot
void FlightTaskAreaMapObject::handleUserDeleteRequest()
{
    QSharedPointer<PlanningProblem> prob = _prob.toStrongRef();
    if (prob.isNull())
        return;

    QSharedPointer<FlightTaskArea> area = _area.toStrongRef();
    if (area.isNull())
        return;

    prob->removeTaskArea(area);
}
