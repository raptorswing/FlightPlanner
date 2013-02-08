#ifndef GPX_GLOBAL_H
#define GPX_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GPX_LIBRARY)
#  define GPXSHARED_EXPORT Q_DECL_EXPORT
#else
#  define GPXSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GPX_GLOBAL_H
