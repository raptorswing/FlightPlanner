#ifndef RRTDISTANCEMETRIC_H
#define RRTDISTANCEMETRIC_H

#include "QKDTreeDistanceMetric.h"

class RRTDistanceMetric : public QKDTreeDistanceMetric
{
public:
    RRTDistanceMetric();

    //virtual from QKDTreeDistanceMetric
    virtual qreal distance(const QVectorND& a, const QVectorND& b);
};

#endif // RRTDISTANCEMETRIC_H
