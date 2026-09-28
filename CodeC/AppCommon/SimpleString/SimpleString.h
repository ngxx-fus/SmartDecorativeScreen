#ifndef __SIMPLE_STRING_H__
#define __SIMPLE_STRING_H__

#include <stdint.h>
#include <stdarg.h>
// #include <vcruntime.h>

/* DEFINITIONS ************************************************************************************/

#define STRING_MAX_SIZE    (0x200U)

/**
 * @brief Instantiates a String_t object backed by a dedicated buffer.
 *
 * @param[out] VarName      Name of the instantiated String_t variable.
 * @param[in]  ReservedSize Allocated buffer capacity in bytes.
 * @param[in]  Str          Preset string literal initializer.
 */
#define StringNew(VarName, ReservedSize, Str)                   \
    char __buffer_str_##VarName##__[ReservedSize] = Str;        \
    String_t VarName = {                                        \
        .buff     = __buffer_str_##VarName##__,                 \
        .size     = (SSize_t)(sizeof(Str) - 1),                 \
        .max_size = (SSize_t)(ReservedSize)                     \
    }

#define NewString(VarName, ReservedSize, Str) StringNew(VarName, ReservedSize, Str)

/* TYPE DEFINITIONS *******************************************************************************/

/**
 * @brief Signed integer type for string lengths, offsets, and search positions.
 */
typedef uint32_t SSize_t;

/**
 * @brief Packed string container holding buffer reference and size boundaries.
 */
typedef struct String_t {
    char*       buff;       /* Pointer to the underlying character buffer */
    SSize_t     size;       /* Current length of valid characters */
    SSize_t     max_size;   /* Maximum allocated buffer capacity */
} String_t;

/* PUBLIC API FUNCTIONS ***************************************************************************/

/**
 * @brief Concatenates N input strings sequentially into destination from a given offset.
 *
 * Output is safely clamped to (Dst->max_size - 2) and null-terminated.
 *
 * @param[in,out] Dst    Target string descriptor.
 * @param[in]     Offset Starting byte offset inside destination buffer.
 * @param[in]     N      Total number of source strings passed.
 * @param[in]     Src0   First source string descriptor.
 * @param[in]     ...    Remaining (N - 1) source string descriptors.
 */
void Str_Concat(String_t *Dst, SSize_t Offset, SSize_t N, String_t Src0, ...);

/**
 * @brief Searches for the first occurrence of a substring within destination limits.
 *
 * @param[in] Dst    Target string descriptor to search within.
 * @param[in] Offset Starting byte index for lookup.
 * @param[in] Src    Substring descriptor to match.
 *
 * @return SSize_t Zero-based start index of match, or -1 if not found.
 */
SSize_t Str_Find(String_t *Dst, SSize_t Offset, String_t Src);

/**
 * @brief Copies source string into destination buffer starting at specified offset.
 *
 * Output is truncated at (Dst->max_size - 2) and null-terminated.
 *
 * @param[in,out] Dst    Destination string descriptor.
 * @param[in]     Offset Write start index.
 * @param[in]     Src    Source string descriptor.
 */
void Str_Copy(String_t *Dst, SSize_t Offset, String_t Src);

/**
 * @brief Repeatedly fills destination buffer with source pattern up to capacity limit.
 *
 * Output is truncated at (Dst->max_size - 2) and null-terminated.
 *
 * @param[in,out] Dst    Destination string descriptor.
 * @param[in]     Offset Write start index.
 * @param[in]     Src    Pattern string descriptor to repeat.
 */
void Str_Fill(String_t *Dst, SSize_t Offset, String_t Src);

/**
 * @brief Formats and writes text into destination buffer without standard library IO.
 *
 * @param[in,out] Dst Target string descriptor.
 * @param[in]     fmt Format specifier string.
 * @param[in]     ... Variable arguments matching format specifiers.
 */
void Str_Printf(String_t *Dst, const char *fmt, ...);

/**
 * @brief Resets destination string to an empty state with zero length.
 *
 * @param[in,out] Dst Target string descriptor to reset.
 */
void Str_Clear(String_t *Dst);

#endif /* __SIMPLE_STRING_H__ */