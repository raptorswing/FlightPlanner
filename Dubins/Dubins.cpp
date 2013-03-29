#include "Dubins.h"

#include "dubinGuts/dubinsSolver.h"

Dubins::Dubins() :
    _angleA(0), _angleB(0), _minTurnRadius(0), _isValid(false)
{
}

Dubins::Dubins(const QVector2D &posA,
               qreal angleA,
               const QVector2D &posB,
               qreal angleB,
               qreal minTurnRadius) :
    _posA(posA.toPointF()),
    _angleA(angleA),
    _posB(posB.toPointF()),
    _angleB(angleB),
    _minTurnRadius(minTurnRadius),
    _isValid(false)
{
    _solvePath();
}

Dubins::Dubins(const QPointF &posA,
               qreal angleA,
               const QPointF &posB,
               qreal angleB,
               qreal minTurnRadius) :
    _posA(posA),
    _angleA(angleA),
    _posB(posB),
    _angleB(angleB),
    _minTurnRadius(minTurnRadius),
    _isValid(false)
{
    _solvePath();
}

bool Dubins::isValid() const
{
    return _isValid;
}

qreal Dubins::length() const
{
    return dubins_path_length(_guts.data());
}

bool Dubins::sample(qreal t, QPointF &outPos, qreal &outAngle)
{
    qreal output[3];
    const int zeroIfGood = dubins_path_sample(_guts.data(), t, output);

    if (zeroIfGood == 0)
    {
        outPos.setX(output[0]);
        outPos.setY(output[1]);
        outAngle = output[2];
    }

    return (zeroIfGood == 0);
}

const QPointF &Dubins::posA() const
{
    return _posA;
}

const QPointF &Dubins::posB() const
{
    return _posB;
}

qreal Dubins::angleA() const
{
    return _angleA;
}

qreal Dubins::angleB() const
{
    return _angleB;
}

void Dubins::setAngleA(qreal a)
{
    this->setAngles(a, this->angleB());
}

void Dubins::setAngleB(qreal b)
{
    this->setAngles(this->angleA(), b);
}

void Dubins::setAngles(qreal a, qreal b)
{
    _angleA = a;
    _angleB = b;
    _solvePath();
}

//private
void Dubins::_solvePath()
{
    qreal start[3] = {_posA.x(), _posA.y(), _angleA};
    qreal end[3] = {_posB.x(), _posB.y(), _angleB};

    _guts = QSharedPointer<DubinsPath>(new DubinsPath());
    const int zeroIfGood = dubins_init(start, end, _minTurnRadius, _guts.data());

    _isValid = false;
    if (zeroIfGood == 0)
        _isValid = true;
}
