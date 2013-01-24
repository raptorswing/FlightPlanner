#include "FlightTask.h"

#include <cmath>

const qreal PI = 3.1415926535897932384626433;
const qreal SQRT2PI = sqrt(2.0*PI);

FlightTask::FlightTask()
{
}

FlightTask::~FlightTask()
{
}

//virtual
bool FlightTask::shortnessRewardApplies() const
{
    return true;
}

qreal FlightTask::priority() const
{
    return 1.0;
}

qreal FlightTask::maxTaskPerformance() const
{
    return 100.0;
}

//protected static
qreal FlightTask::normal(qreal x, qreal stdDev, qreal scaleFactor)
{
    qreal expPart = exp(-0.5 * pow(x / stdDev, 2.0) / scaleFactor);
    qreal otherPart = stdDev * SQRT2PI;
    return (1.0 / otherPart) * expPart;
}
