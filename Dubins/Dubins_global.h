#ifndef DUBINS_GLOBAL_H
#define DUBINS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DUBINS_LIBRARY)
#  define DUBINSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DUBINSSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DUBINS_GLOBAL_H
