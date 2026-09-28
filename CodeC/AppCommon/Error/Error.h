#ifndef __ERROR_H__
#define __ERROR_H__

#include "stdint.h"

typedef int32_t Return_t;
typedef int32_t Error_t;

typedef enum Error_e {
    STAT_SUCCESS                 = 0,   ///< Success
    STAT_ERROR                   = 1,  ///< Generic error
    STAT_ERR_NULL                = 2,  ///< Null pointer passed
    STAT_ERR_MALLOC_FAILED       = 3,  ///< Memory allocation failed
    STAT_ERR_TIMEOUT             = 4,  ///< Timeout occurred
    STAT_ERR_BUSY                = 5,  ///< Resource/device is busy
    STAT_ERR_INVALID_ARG         = 6,  ///< Invalid argument
    STAT_ERR_INVALID_STATE       = 7,  ///< Invalid state (e.g. not initialized)
    STAT_ERR_INVALID_SIZE        = 8,  ///< Invalid size provided
    STAT_ERR_OVERFLOW            = 9,  ///< Buffer/variable overflow
    STAT_ERR_UNDERFLOW           = 10, ///< Buffer/variable underflow
    STAT_ERR_NOT_FOUND           = 11, ///< Resource not found
    STAT_ERR_ALREADY_EXISTS      = 12, ///< Resource already exists
    STAT_ERR_NOT_IMPLEMENTED     = 13, ///< Functionality not implemented
    STAT_ERR_UNSUPPORTED         = 14, ///< Unsupported operation
    STAT_ERR_IO                  = 15, ///< Input/Output error
    STAT_ERR_PERMISSION          = 16, ///< Permission denied
    STAT_ERR_CRC                 = 17, ///< CRC check failed (data corrupted)
    STAT_ERR_INIT_FAILED         = 18, ///< Initialization failed
    STAT_ERR_PSRAM_FAILED        = 19, ///< PSRAM failure
    STAT_MAX_NUM
} Error_e;

extern const char* STR_STAT(Error_e code);

#endif