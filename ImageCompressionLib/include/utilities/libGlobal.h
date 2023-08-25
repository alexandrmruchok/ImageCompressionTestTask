#ifndef LIBGLOBAL_H
#define LIBGLOBAL_H

 #if defined(SHAREDLIB_LIBRARY)
 #  define SHAREDLIB_EXPORT __declspec(dllexport)
 #else
 #  define SHAREDLIB_EXPORT __declspec(dllimport)
 #endif

#endif // LIBGLOBAL_H
