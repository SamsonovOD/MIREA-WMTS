#ifndef WMTSLIB_GLOBAL_H
#define WMTSLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(WMTSLIB_LIBRARY)
#  define WMTSLIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define WMTSLIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // WMTSLIB_GLOBAL_H
