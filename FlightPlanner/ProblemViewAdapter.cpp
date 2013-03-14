#include "ProblemViewAdapter.h"

#include <QtDebug>
#include <QMutableSetIterator>

ProblemViewAdapter::ProblemViewAdapter(QSharedPointer<PlanningProblem> model,
                                       MapGraphicsScene *view,
                                       QObject *parent) :
    QObject(parent), _view(view)
{
    this->setModel(model);
}

//public slot
void ProblemViewAdapter::setModel(QSharedPointer<PlanningProblem> model)
{
    if (model.isNull())
        return;

    //Destroy things in current model
    if (!_model.isNull())
    {
        /*
          No need to actually remove objects! Just delete them.
          MapGraphics will take care of removing when it detects the deletion.
        */

        if (!_startPosObject.isNull())
            _startPosObject.clear();

        _areaObjects.clear();
    }

    _model = model;

    if (_model.isNull())
        return;

    PlanningProblem * conv = _model.data(); //For convenience
    connect(conv,
            SIGNAL(startingPositionChanged(Position)),
            this,
            SLOT(handleStartingPositionChanged(Position)));

    connect(conv,
            SIGNAL(startingOrientationChanged(UAVOrientation)),
            this,
            SLOT(handleStartingOrientationChanged(UAVOrientation)));

    connect(conv,
            SIGNAL(flighTaskAreaAdded(QSharedPointer<FlightTaskArea>)),
            this,
            SLOT(handleFlightTaskAreaAdded(QSharedPointer<FlightTaskArea>)));
    connect(conv,
            SIGNAL(flightTaskAreaRemoved(QSharedPointer<FlightTaskArea>)),
            this,
            SLOT(handleFlightTaskAreaRemoved(QSharedPointer<FlightTaskArea>)));

    //Fire some fake events to force generation of the map graphics objects
    this->handleStartingPositionChanged(_model->startingPosition());
    this->handleStartingOrientationChanged(_model->startingOrientation());
    foreach(const QSharedPointer<FlightTaskArea>& area, _model->areas())
        this->handleFlightTaskAreaAdded(area);
}

//private slot
void ProblemViewAdapter::handleStartingPositionChanged(const Position &pos)
{
    if (_startPosObject.isNull())
    {
        _startPosObject = QSharedPointer<StartPosMapObject>(new StartPosMapObject(_model.toWeakRef()));
        _view->addObject(_startPosObject.data());
        _startPosObject->setPos(pos.lonLat());
    }
}

//private slot
void ProblemViewAdapter::handleStartingOrientationChanged(const UAVOrientation &orientation)
{
    _startPosObject->setOrientation(orientation);
}

//private slot
void ProblemViewAdapter::handleFlightTaskAreaAdded(const QSharedPointer<FlightTaskArea> &area)
{
    //This object needs the model so that it can delete the area in the model when needed
    QSharedPointer<FlightTaskAreaMapObject> obj(new FlightTaskAreaMapObject(_model.toWeakRef(),
                                                                            area));
    _areaObjects.insert(obj);
    _view->addObject(obj.data());
}

//private slot
void ProblemViewAdapter::handleFlightTaskAreaRemoved(const QSharedPointer<FlightTaskArea> &area)
{
    if (area.isNull())
        return;

    QWeakPointer<FlightTaskArea> weak = area.toWeakRef();

    QMutableSetIterator<QSharedPointer<FlightTaskAreaMapObject> > iter(_areaObjects);
    while (iter.hasNext())
    {
        QSharedPointer<FlightTaskAreaMapObject> item = iter.next();
        if (item->flightTaskArea() == weak)
            iter.remove();
    }
}
