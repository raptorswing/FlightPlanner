#ifndef GPXPOINT_H
#define GPXPOINT_H

#include "GPX_global.h"

#include <QDateTime>

/**
 * @brief The GPXPoint class is a glorified struct. I'm sorry.
 */
class GPXSHARED_EXPORT GPXPoint
{
public:
    GPXPoint();
    ~GPXPoint();

    double longitude;
    double latitude;
    double height;
    QDateTime time;
};

#endif // GPXPOINT_H
