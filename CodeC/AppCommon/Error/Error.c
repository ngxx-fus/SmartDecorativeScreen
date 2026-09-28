#include "Error.h"

/**
 * @brief Static lookup table mapping error codes to description strings in flash.
 */
static const char* const s_error_str_table[STAT_MAX_NUM] = {
    [STAT_SUCCESS]            = "Success",
    [STAT_ERROR]              = "Generic error",
    [STAT_ERR_NULL]           = "Null pointer passed",
    [STAT_ERR_MALLOC_FAILED]  = "Memory allocation failed",
    [STAT_ERR_TIMEOUT]        = "Timeout occurred",
    [STAT_ERR_BUSY]           = "Resource/device is busy",
    [STAT_ERR_INVALID_ARG]    = "Invalid argument",
    [STAT_ERR_INVALID_STATE]  = "Invalid state (not initialized)",
    [STAT_ERR_INVALID_SIZE]   = "Invalid size provided",
    [STAT_ERR_OVERFLOW]       = "Buffer/variable overflow",
    [STAT_ERR_UNDERFLOW]      = "Buffer/variable underflow",
    [STAT_ERR_NOT_FOUND]      = "Resource not found",
    [STAT_ERR_ALREADY_EXISTS] = "Resource already exists",
    [STAT_ERR_NOT_IMPLEMENTED]= "Functionality not implemented",
    [STAT_ERR_UNSUPPORTED]    = "Unsupported operation",
    [STAT_ERR_IO]             = "Input/Output error",
    [STAT_ERR_PERMISSION]     = "Permission denied",
    [STAT_ERR_CRC]            = "CRC check failed",
    [STAT_ERR_INIT_FAILED]    = "Initialization failed",
    [STAT_ERR_PSRAM_FAILED]   = "PSRAM failure"
};

/* Public API implementation matching the external signature */
const char* STR_STAT(Error_e code)
{
    /* Validate error code boundaries to prevent out-of-bounds access */
    if ((int)code < 0 || code >= STAT_MAX_NUM) 
    {
        /* Return fallback string for invalid error codes */
        return "Unknown error code";
    }

    /* Return the corresponding error description string from flash */
    return s_error_str_table[code];
}