#ifndef SQL_LOGGER_H
#define SQL_LOGGER_H


#ifdef __ANDROID_API__
#include <android/log.h>
#endif

#ifdef __ANDROID_API__
#define SQL_LOG(s, r, p) __android_log_print(ANDROID_LOG_ERROR, "SQLite", s, r, p);
#else

#if __has_include("../Logger.h")
#	include "../Logger.h"
#	define SQL_LOG(s, r, p) MY_LOG_ERROR(s, r, p);
#else
#	define SQL_LOG(s, r, p) printf(s, r, p)
#endif

#endif
