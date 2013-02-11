#ifndef RRTDISTANCEMETRIC_H
#define RRTDISTANCEMETRIC_H

#include "QKDTreeDistanceMetric.h"

class RRTDistanceMetric : public QKDTreeDistanceMetric
{
public:
    RRTDistanceMetric(qreal latitude, qreal minTurnRadius);

    //virtual from QKDTreeDistanceMetric
    virtual qreal distance(const QVectorND& a, const QVectorND& b);

private:
    const qreal _lonPerMeter;
    const qreal _latPerMeter;
    const qreal _minTurnRadius;
};

#endif // RRTDISTANCEMETRIC_H
