#pragma execution_character_set("utf-8")

#ifndef QTIMAGEVIEWER_GLOBAL_H
#define QTIMAGEVIEWER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QTIMAGEVIEWER_LIBRARY)
#define QTIMAGEVIEWER_EXPORT Q_DECL_EXPORT
#else
#define QTIMAGEVIEWER_EXPORT Q_DECL_IMPORT
#endif

#endif // QTIMAGEVIEWER_GLOBAL_H
