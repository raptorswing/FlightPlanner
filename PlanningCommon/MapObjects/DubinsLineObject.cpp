#include "DubinsLineObject.h"

#include <QtDebug>
#include <QTimer>

#include "guts/Conversions.h"

const qreal GRANULARITY = 10.0;

DubinsLineObject::DubinsLineObject(const Position &rootPos,
                                   const Dubins &dubins,
                                   qreal dubinsStart,
                                   MapGraphicsObject *parent) :
    LineObject(Position(), Position(), parent),
    _rootPos(rootPos), _dubins(dubins), _dubinsStart(dubinsStart)
{
    QTimer::singleShot(1, this, SLOT(updateDubins()));
    //this->setDubins(rootPos, dubins, dubinsStart);
}

DubinsLineObject::~DubinsLineObject()
{
    while (!_next.isNull())
    {
        _next->deleteLater();
        _next = _next->_next;
    }
}

void DubinsLineObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(Qt::red);
    LineObject::paint(painter, option, widget);
}

//public slot
void DubinsLineObject::setDubins(Position rootPos,
                                 Dubins dubins,
                                 qreal dubinsStart)
{
    if (!dubins.isValid())
    {
        qDebug() << "Invalid dubins";
        return;
    }

    const qreal lonPerMeter = Conversions::degreesLonPerMeter(rootPos.latitude());
    const qreal latPerMeter = Conversions::degreesLatPerMeter(rootPos.latitude());

    const qreal startTime = dubinsStart;
    const qreal endTime = qMin<qreal>(dubinsStart + GRANULARITY,
                                      dubins.length() - 0.5);

    QPointF startPosMeters, endPosMeters;
    qreal outAngle;
    if (!dubins.sample(startTime, startPosMeters, outAngle))
        qDebug() << "Bad start sample";
    if (!dubins.sample(endTime, endPosMeters, outAngle))
        qDebug() << "Bad end sample";

    Position startPos(rootPos.longitude() + startPosMeters.x() * lonPerMeter,
                      rootPos.latitude() + startPosMeters.y() * latPerMeter);
    Position endPos(rootPos.longitude() + endPosMeters.x() * lonPerMeter,
                    rootPos.latitude() + endPosMeters.y() * latPerMeter);

    this->setEndPoints(startPos, endPos);

    if (endTime >= dubins.length() - 0.6 && !_next.isNull())
    {
        while (!_next.isNull())
        {
            _next->deleteLater();
            _next = _next->_next;
        }
    }
    else if (endTime < dubins.length() - 0.6)
    {
        if (_next.isNull())
        {
            _next = new DubinsLineObject(rootPos, dubins, endTime);
            this->newObjectGenerated(_next);
        }
        else
            _next->setDubins(rootPos, dubins, endTime);
    }
    this->redrawRequested();
}

//private slot
void DubinsLineObject::updateDubins()
{
    this->setDubins(_rootPos, _dubins, _dubinsStart);
}
