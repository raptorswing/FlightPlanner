#include "DubinsLineObject.h"

#include <QtDebug>
#include <QtCore>

#include "guts/Conversions.h"

const qreal GRANULARITY = 20.0;

DubinsLineObject::DubinsLineObject(const Position &rootPos,
                                   const Dubins &dubins, qreal thickness,
                                   MapGraphicsObject *parent) :
    MapGraphicsObject(false, parent),
    _rootPos(rootPos), _dubins(dubins), _thickness(thickness)
{
    this->setDubins(rootPos, dubins);
}

DubinsLineObject::~DubinsLineObject()
{
}

//pure-virtual from MapGraphicsObject
QRectF DubinsLineObject::boundingRect() const
{
    return _boundingRect;
}

//pure-virtual from MapGraphicsObject
void DubinsLineObject::paint(QPainter *painter,
                             const QStyleOptionGraphicsItem *option,
                             QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setRenderHint(QPainter::Antialiasing, true);
    QPen pen = painter->pen();
    pen.setWidthF(_thickness);
    painter->setPen(pen);

    for (int i = 1; i < _drawOffsets.size() - 1; i++)
    {
        const QPointF& next = _drawOffsets.at(i);
        const QPointF& prev = _drawOffsets.at(i-1);

        painter->drawLine(prev, next);
    }
}

qreal DubinsLineObject::thickness() const
{
    return _thickness;
}

void DubinsLineObject::setThickness(qreal nThick)
{
    _thickness = nThick;
    this->redrawRequested();
}

//public slot
void DubinsLineObject::setDubins(Position rootPos,
                                 Dubins dubins)
{
    /*
     *We ease the turning radius a little bit for display to combat "false loops"
     *created by floating point error
    */
    dubins.setMinTurnRadius(dubins.minTurnRadius() * 0.99);

    _rootPos = rootPos;
    _dubins = dubins;
    _drawOffsets.clear();
    if (!dubins.isValid())
    {
        qDebug() << "Invalid dubins";
        return;
    }

    qreal maxX = rootPos.longitude();
    qreal maxY = rootPos.latitude();
    qreal minX = maxX;
    qreal minY = maxY;

    QList<Position> tempPositions;

    const int numChecks = qCeil(dubins.length() / GRANULARITY) + 2;
    for (int i = 0; i < numChecks; i++)
    {
        const qreal dubinPos = qMin<qreal>(GRANULARITY * i,
                                           dubins.length() - 0.1);
        QPointF offset;
        qreal outAngle;
        dubins.sample(dubinPos, offset, outAngle);

        const Position latLon = rootPos.flatOffsetToPosition(offset);
        tempPositions.append(latLon);

        if (latLon.longitude() > maxX)
            maxX = latLon.longitude();
        if (latLon.longitude() < minX)
            minX = latLon.longitude();
        if (latLon.latitude() > maxY)
            maxY = latLon.latitude();
        if (latLon.latitude() < minY)
            minY = latLon.latitude();
    }

    const QRectF latLonRect(minX, minY, maxX - minX, maxY - minY);
    const Position center(latLonRect.center().x(), latLonRect.center().y());

    foreach(const Position& tempPos, tempPositions)
    {
        QVector2D offset = center.flatOffsetMeters(tempPos);
        _drawOffsets.append(offset.toPointF());
    }

    const qreal lonPerMeter = Conversions::degreesLonPerMeter(center.latitude());
    const qreal latPerMeter = Conversions::degreesLatPerMeter(center.latitude());
    const qreal widthMeters = qMax<qreal>(qAbs<qreal>(maxX - minX) / lonPerMeter,
                                          5.0);
    const qreal heightMeters = qMax<qreal>(qAbs<qreal>(maxY - minY) / latPerMeter,
                                           5.0);
    _boundingRect = QRectF(-1*widthMeters, -1*heightMeters,
                           2*widthMeters, 2*heightMeters);


    this->setPos(latLonRect.center());
    this->redrawRequested();
}

//private slot
void DubinsLineObject::updateDubins()
{
    this->setDubins(_rootPos, _dubins);
}
