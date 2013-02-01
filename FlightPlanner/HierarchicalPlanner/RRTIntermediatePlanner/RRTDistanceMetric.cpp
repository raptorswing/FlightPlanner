#include "RRTDistanceMetric.h"

#include <cmath>

#include "guts/Conversions.h"

RRTDistanceMetric::RRTDistanceMetric(qreal latitude) :
    _lonPerMeter(Conversions::degreesLonPerMeter(latitude)),
    _latPerMeter(Conversions::degreesLatPerMeter(latitude))
{
}

//virtual from QKDTreeDistanceMetric
qreal RRTDistanceMetric::distance(const QVectorND &a, const QVectorND &b)
{
    const qreal lonMeters = (a.val(0) - b.val(0)) / _lonPerMeter;
    const qreal latMeters = (a.val(1) - b.val(1)) / _latPerMeter;

    const qreal distanceInMeters = sqrt(lonMeters*lonMeters + latMeters*latMeters);

    const qreal smallAngle1 = qMin<qreal>(a.val(2), b.val(2));
    const qreal smallAngle2 = smallAngle1 + (2.0 * 3.14159265);
    const qreal bigAngle = qMax<qreal>(a.val(2), b.val(2));

    const qreal radianDiff = qMin<qreal>(qAbs<qreal>(smallAngle1 - bigAngle),
                                         qAbs<qreal>(smallAngle2 - bigAngle));
    const qreal degreeDiff = radianDiff * (180.0 / 3.14159265);

    qreal totalCost = distanceInMeters;
    //if (distanceInMeters < 100.0)
    totalCost += 0.5 * degreeDiff;

    return totalCost;
}
