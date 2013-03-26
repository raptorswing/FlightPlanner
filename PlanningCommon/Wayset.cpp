#include "Wayset.h"
#include "Dubins.h"

#include <QtDebug>
#include <QVector2D>

Wayset::Wayset()
{
}

Wayset::Wayset(const QList<Position> &waypoints) :
    _waypoints(waypoints)
{
}

const Position &Wayset::at(int i) const
{
    if (i < 0 || i >= this->size())
        qWarning() << "Index" << i << "out of bounds!";
    return _waypoints.at(i);
}

Position &Wayset::first()
{
    return _waypoints.first();
}

const Position &Wayset::first() const
{
    return _waypoints.last();
}

Position &Wayset::last()
{
    return _waypoints.last();
}

const Position &Wayset::last() const
{
    return _waypoints.last();
}

int Wayset::count() const
{
    return _waypoints.size();
}

int Wayset::size() const
{
    return _waypoints.size();
}

bool Wayset::isEmpty() const
{
    return _waypoints.isEmpty();
}

qreal Wayset::lengthMeters(const UAVParameters &uavParams) const
{
    qreal toRet = 0.0;

    for (int i = 0; i < this->count() - 1; i++)
    {
        const Position& a = this->at(i);
        const Position& b = this->at(i + 1);

        const qreal startAngle = a.angleTo(b);
        qreal endAngle = startAngle;
        if (i < this->count() - 2)
            endAngle = b.angleTo(this->at(i + 2));

        const QVector2D startPos(0.0, 0.0);
        const QVector2D endPos = a.flatOffsetMeters(b);

        Dubins dubins(startPos, startAngle, endPos, endAngle, uavParams.minTurningRadius());
        if (!dubins.isValid())
        {
            qWarning() << "Invalid dubins in lengthMeters()";
            continue;
        }
        toRet += dubins.length();
    }

    return toRet;
}

qreal Wayset::timeToFly(const UAVParameters &uavParams) const
{
    const qreal meters = this->lengthMeters(uavParams);
    const qreal ttf = meters / uavParams.airspeed();
    return ttf;
}

void Wayset::clear()
{
    _waypoints.clear();
}

void Wayset::append(const Position &pos)
{
    _waypoints.append(pos);
}

void Wayset::append(const Wayset &wayset)
{
    _waypoints.append(wayset.waypoints());
}

void Wayset::prepend(const Position &pos)
{
    _waypoints.prepend(pos);
}

const QList<Position> &Wayset::waypoints() const
{
    return _waypoints;
}
