#ifndef RRTDISTANCEMETRIC_H
#define RRTDISTANCEMETRIC_H

#include "QKDTreeDistanceMetric.h"

class RRTDistanceMetric : public QKDTreeDistanceMetric
{
public:
    RRTDistanceMetric(qreal latitude);

    //virtual from QKDTreeDistanceMetric
    virtual qreal distance(const QVectorND& a, const QVectorND& b);

private:
    const qreal _lonPerMeter;
    const qreal _latPerMeter;
};

#endif // RRTDISTANCEMETRIC_H
