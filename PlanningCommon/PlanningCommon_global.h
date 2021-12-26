#ifndef PLANNINGCOMMON_GLOBAL_H
#define PLANNINGCOMMON_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PLANNINGCOMMON_LIBRARY)
#  define PLANNINGCOMMONSHARED_EXPORT Q_DECL_EXPORT
#else
#  define PLANNINGCOMMONSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // PLANNINGCOMMON_GLOBAL_H
