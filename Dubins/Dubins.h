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
    Dubins(const QVector2D& firstPosition,
           qreal firstAngle,
           const QVector2D& secondPosition,
           qreal secondAngle,
           qreal minTurnRadius);

    Dubins(const QPointF& firstPosition,
           qreal firstAngle,
           const QPointF& secondPosition,
           qreal secondAngle,
           qreal minTurnRadius);

    bool isValid() const;

    qreal length() const;
    bool sample(qreal t, QPointF& outPos, qreal& outAngle) const;

    const QPointF& firstPosition() const;
    const QPointF& secondPosition() const;

    void setFirstPosition(const QPointF& firstPos);
    void setSecondPosition(const QPointF& secondPos);
    void setPositions(const QPointF& first, const QPointF& second);

    qreal firstAngle() const;
    qreal secondAngle() const;

    void setFirstAngle(qreal a);
    void setSecondAngle(qreal b);
    void setAngles(qreal a, qreal b);

    qreal minTurnRadius() const;
    void setMinTurnRadius(qreal nRadius);


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
