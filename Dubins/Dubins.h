#ifndef DUBINS_H
#define DUBINS_H

#include <QVector2D>
#include <QPointF>
#include <QSharedPointer>

#include "Dubins_global.h"

#include "dubinGuts/dubinsSolver.h"

class DUBINSSHARED_EXPORT Dubins
{
public:
    Dubins();
    Dubins(const QVector2D& posA,
           qreal angleA,
           const QVector2D& posB,
           qreal angleB,
           qreal minTurnRadius);

    Dubins(const QPointF& posA,
           qreal angleA,
           const QPointF& posB,
           qreal angleB,
           qreal minTurnRadius);

    bool isValid() const;

    qreal length() const;
    bool sample(qreal t, QPointF& outPos, qreal& outAngle);


private:
    void _solvePath();

    QPointF _posA;
    qreal _angleA;

    QPointF _posB;
    qreal _angleB;

    qreal _minTurnRadius;

    bool _isValid;

    QSharedPointer<DubinsPath> _guts;
};

#endif // DUBINS_H
