#ifndef __LOG_H__
#define __LOG_H__

#include "stdint.h"
#include "stdarg.h"

/* Local source */
#include "../SimpleString/SimpleString.h"
#include <stdint.h>

/* @brief Function pointer signature for log engine callback. */
typedef void (*LogEngine_t)(const SSize_t len, const char *msg);
typedef void (*LogFunction_t)(const char * FILE, uint32_t LINE, const char * fmt, ...);


typedef struct SysLogAPI_t {
	LogEngine_t 	Engine;
	LogFunction_t   Info;
	LogFunction_t   Warn;
	LogFunction_t   Error;
	LogFunction_t   Entry;
	LogFunction_t   Exit;
	void * 			ExtendConfig;
} SysLogAPI_t;

extern SysLogAPI_t g_SysLog;

#define SysInfo(fmt, ... ) g_SysLog.Info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define SysWarn(fmt, ... ) g_SysLog.Warn(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define SysError(fmt, ... ) g_SysLog.Error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define SysEntry(fmt, ... ) g_SysLog.Entry(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define SysExit(fmt, ... ) g_SysLog.Exit(__FILE__, __LINE__, fmt, 	##__VA_ARGS__)

#endif