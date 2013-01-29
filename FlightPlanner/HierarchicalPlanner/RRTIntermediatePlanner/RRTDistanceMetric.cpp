#include "RRTDistanceMetric.h"

#include <cmath>

#include "guts/Conversions.h"

RRTDistanceMetric::RRTDistanceMetric()
{
}

//virtual from QKDTreeDistanceMetric
qreal RRTDistanceMetric::distance(const QVectorND &a, const QVectorND &b)
{
    const qreal lonPerMeter = Conversions::degreesLonPerMeter(a.val(1));
    const qreal latPerMeter = Conversions::degreesLatPerMeter(a.val(1));

    const qreal lonMeters = (a.val(0) - b.val(0)) / lonPerMeter;
    const qreal latMeters = (a.val(1) - b.val(1)) / latPerMeter;

    const qreal distanceInMeters = lonMeters*lonMeters + latMeters*latMeters;

    return distanceInMeters;
}
