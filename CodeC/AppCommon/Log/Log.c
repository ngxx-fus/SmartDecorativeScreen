#include "Log.h"

/* Global source */
#include <stdarg.h>
#include <stdint.h>
#include <stdatomic.h>

/* Local source */
#include "../SimpleString/SimpleString.h"

/* DEFINITIONS ************************************************************************************/

#ifndef SYS_LOG_ENGINE
    /* Default log-engine is null, request a function take size and char buffer */
    #define SYS_LOG_ENGINE          NULL
    #define SYS_LOG_ENGINE_DISABLED 1
#endif /* SYS_LOG_ENGINE */

#define MAX_SIZE_BUFFER             512U

/* Spinlock lock state values */
#define SYSLOG_UNLOCKED             (0U)
#define SYSLOG_LOCKED               (1U)

/* PRIVATE VARIABLES ******************************************************************************/

/* Atomic spinlock state dedicated to protecting log engine dispatch */
_Atomic uint32_t SysLogLockState = SYSLOG_UNLOCKED;

/* Buffer for log string formatting */
StringNew(g_SysLogStr, MAX_SIZE_BUFFER, "");

/* PRIVATE FUNCTIONS ******************************************************************************/

/*
 * @brief Acquires spinlock dedicated to LogEngine.
 */
static void LogEngine_Lock(void)
{
    uint32_t expected = SYSLOG_UNLOCKED;

    /* Spin until lock transitions from UNLOCKED to LOCKED */
    while (!atomic_compare_exchange_weak(&SysLogLockState, &expected, SYSLOG_LOCKED)) {
        expected = SYSLOG_UNLOCKED;
    } /* end while */
}

/*
 * @brief Releases spinlock dedicated to LogEngine.
 */
static void LogEngine_Unlock(void)
{
    /* Atomically set spinlock back to UNLOCKED */
    atomic_store(&SysLogLockState, SYSLOG_UNLOCKED);
}

/*
 * @brief Transmits formatted string buffer via LogEngine with exclusive lock protection.
 * @param buf Pointer to character buffer.
 * @param size Length of valid characters.
 */
static void LogEngine_Transmit(const char *buf, SSize_t size)
{
    /* Steering check for engine availability */
    if (g_SysLog.Engine == NULL) {
        /* Abort if engine is absent */
        return;
    } /* end if */

    /* Acquire engine spinlock */
    LogEngine_Lock();

    /* Transmit character stream */
    g_SysLog.Engine(size, buf);

    /* Release engine spinlock */
    LogEngine_Unlock();
}

/*
 * @brief Primary logging function that formats message and delegates to active log engine.
 * @param msg Format string for the log output.
 */
void CoreLog(const char * msg, ...)
{
#if (SYS_LOG_ENGINE_DISABLED != 1)
    /* Steering check for engine availability */
    if (g_SysLog.Engine == NULL) {
        /* Abort if no log engine is registered */
        return;
    } /* end if */

    /* Reset log string descriptor before reuse */
    Str_Clear(&g_SysLogStr);

    va_list args;

    /* Initialize va_list with the last fixed parameter */
    va_start(args, msg);

    /* Format log message into internal String buffer */
    Str_Printf(&g_SysLogStr, msg, args);

    /* Clean up va_list */
    va_end(args);

    /* Transmit formatted string through active log engine with spinlock protection */
    LogEngine_Transmit(g_SysLogStr.buff, g_SysLogStr.size);
#else 
    /* There is no log engine enabled */
    return;
#endif /* (SYS_LOG_ENGINE_DISABLED != 1) */
}

/*
 * @brief Dispatches informational log messages.
 * @param FILE Source file path/name.
 * @param LINE Source file line string.
 * @param fmt Format string.
 * @param ... Variable argument list.
 */
void Prv_SysInfo(const char * FILE, uint32_t LINE, const char * fmt, ...)
{
#if (SYS_LOG_ENGINE_DISABLED != 1)
    /* Steering check for engine availability */
    if (g_SysLog.Engine == NULL) {
        /* Exit if no log engine is registered */
        return;
    } /* end if */

    Str_Clear(&g_SysLogStr);

    /* Prefix log category and origin */
    Str_Printf(&g_SysLogStr, "[INFO] [%s:%d] ", FILE, LINE);

    va_list args;
    va_start(args, fmt);

    /* Append formatted message starting from current offset */
    StringNew(user_msg, MAX_SIZE_BUFFER, "");
    Str_Printf(&user_msg, fmt, args);
    va_end(args);

    Str_Concat(&g_SysLogStr, g_SysLogStr.size, 1, user_msg);

    /* Append newline */
    StringNew(newline, 4, "\r\n");
    Str_Concat(&g_SysLogStr, g_SysLogStr.size, 1, newline);

    /* Transmit payload */
    LogEngine_Transmit(g_SysLogStr.buff, g_SysLogStr.size);
#else
    return;
#endif /* (SYS_LOG_ENGINE_DISABLED != 1) */
}

/*
 * @brief Dispatches error log messages.
 * @param FILE Source file path/name.
 * @param LINE Source file line string.
 * @param fmt Format string.
 * @param ... Variable argument list.
 */
void Prv_SysError(const char * FILE, uint32_t LINE, const char * fmt, ...)
{
#if (SYS_LOG_ENGINE_DISABLED != 1)
    /* Steering check for engine availability */
    if (g_SysLog.Engine == NULL) {
        /* Exit if no log engine is registered */
        return;
    } /* end if */

    Str_Clear(&g_SysLogStr);

    /* Prefix log category and origin */
    Str_Printf(&g_SysLogStr, "[ERROR] [%s:%d] ", FILE, LINE);

    va_list args;
    va_start(args, fmt);

    StringNew(user_msg, MAX_SIZE_BUFFER, "");
    Str_Printf(&user_msg, fmt, args);
    va_end(args);

    Str_Concat(&g_SysLogStr, g_SysLogStr.size, 1, user_msg);

    StringNew(newline, 4, "\r\n");
    Str_Concat(&g_SysLogStr, g_SysLogStr.size, 1, newline);

    /* Transmit payload */
    LogEngine_Transmit(g_SysLogStr.buff, g_SysLogStr.size);
#else
    return;
#endif /* (SYS_LOG_ENGINE_DISABLED != 1) */
}

/*
 * @brief Dispatches warning log messages.
 * @param FILE Source file path/name.
 * @param LINE Source file line string.
 * @param fmt Format string.
 * @param ... Variable argument list.
 */
void Prv_SysWarn(const char * FILE, uint32_t LINE, const char * fmt, ...)
{
#if (SYS_LOG_ENGINE_DISABLED != 1)
    /* Steering check for engine availability */
    if (g_SysLog.Engine == NULL) {
        /* Exit if no log engine is registered */
        return;
    } /* end if */

    Str_Clear(&g_SysLogStr);

    /* Prefix log category and origin */
    Str_Printf(&g_SysLogStr, "[WARN] [%s:%d] ", FILE, LINE);

    va_list args;
    va_start(args, fmt);

    StringNew(user_msg, MAX_SIZE_BUFFER, "");
    Str_Printf(&user_msg, fmt, args);
    va_end(args);

    Str_Concat(&g_SysLogStr, g_SysLogStr.size, 1, user_msg);

    StringNew(newline, 4, "\r\n");
    Str_Concat(&g_SysLogStr, g_SysLogStr.size, 1, newline);

    /* Transmit payload */
    LogEngine_Transmit(g_SysLogStr.buff, g_SysLogStr.size);
#else
    return;
#endif /* (SYS_LOG_ENGINE_DISABLED != 1) */
}

/*
 * @brief Dispatches function entry tracking log messages.
 * @param FILE Source file path/name.
 * @param LINE Source file line string.
 * @param fmt Format string.
 * @param ... Variable argument list.
 */
void Prv_SysEntry(const char * FILE, uint32_t LINE, const char * fmt, ...)
{
#if (SYS_LOG_ENGINE_DISABLED != 1)
    /* Steering check for engine availability */
    if (g_SysLog.Engine == NULL) {
        /* Exit if no log engine is registered */
        return;
    } /* end if */

    Str_Clear(&g_SysLogStr);

    /* Prefix log category and origin */
    Str_Printf(&g_SysLogStr, "[ENTRY] [%s:%d] >> ", FILE, LINE);

    va_list args;
    va_start(args, fmt);

    StringNew(user_msg, MAX_SIZE_BUFFER, "");
    Str_Printf(&user_msg, fmt, args);
    va_end(args);

    Str_Concat(&g_SysLogStr, g_SysLogStr.size, 1, user_msg);

    StringNew(newline, 4, "\r\n");
    Str_Concat(&g_SysLogStr, g_SysLogStr.size, 1, newline);

    /* Transmit payload */
    LogEngine_Transmit(g_SysLogStr.buff, g_SysLogStr.size);
#else
    return;
#endif /* (SYS_LOG_ENGINE_DISABLED != 1) */
}

/*
 * @brief Dispatches function exit tracking log messages.
 * @param FILE Source file path/name.
 * @param LINE Source file line string.
 * @param fmt Format string.
 * @param ... Variable argument list.
 */
void Prv_SysExit(const char * FILE, uint32_t LINE, const char * fmt, ...)
{
#if (SYS_LOG_ENGINE_DISABLED != 1)
    /* Steering check for engine availability */
    if (g_SysLog.Engine == NULL) {
        /* Exit if no log engine is registered */
        return;
    } /* end if */

    Str_Clear(&g_SysLogStr);

    /* Prefix log category and origin */
    Str_Printf(&g_SysLogStr, "[EXIT] [%s:%d] << ", FILE, LINE);

    va_list args;
    va_start(args, fmt);

    StringNew(user_msg, MAX_SIZE_BUFFER, "");
    Str_Printf(&user_msg, fmt, args);
    va_end(args);

    Str_Concat(&g_SysLogStr, g_SysLogStr.size, 1, user_msg);

    StringNew(newline, 4, "\r\n");
    Str_Concat(&g_SysLogStr, g_SysLogStr.size, 1, newline);

    /* Transmit payload */
    LogEngine_Transmit(g_SysLogStr.buff, g_SysLogStr.size);
#else
    return;
#endif /* (SYS_LOG_ENGINE_DISABLED != 1) */
}

/* Public API ********************************************************************************************/

/* System log APIs */
SysLogAPI_t g_SysLog = {
    .Engine         = SYS_LOG_ENGINE,
    .Info           = Prv_SysInfo,
    .Warn           = Prv_SysWarn,
    .Error          = Prv_SysError,
    .Entry          = Prv_SysEntry,
    .Exit           = Prv_SysExit,
    .ExtendConfig   = NULL
};