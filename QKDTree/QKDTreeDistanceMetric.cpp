#include "QKDTreeDistanceMetric.h"

#include <QtGlobal>
#include <cmath>

QKDTreeDistanceMetric::QKDTreeDistanceMetric()
{
}

QKDTreeDistanceMetric::~QKDTreeDistanceMetric()
{
}

qreal QKDTreeDistanceMetric::distance(const QKDTreeNode *const a, const QKDTreeNode *const b)
{
    return this->distance(a->position(), b->position());
}

//virtual - this one returns euclidean distance
qreal QKDTreeDistanceMetric::distance(const QVectorND &a, const QVectorND &b)
{
    qreal toRet = 0.0;
    for (int i = 0; i < qMin<int>(a.dimension(), b.dimension()); i++)
    {
        const qreal diff = a[i] - b[i];
        toRet += diff * diff;
    }
    return toRet;
}
