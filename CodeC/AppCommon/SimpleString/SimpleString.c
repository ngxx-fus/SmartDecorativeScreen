#include "SimpleString.h"
#include <stddef.h>

/* PRIVATE DEFINITIONS AND TYPES *******************************************************************/

#define FLOAT_PRECISION_DEFAULT   (6U)

/* PRIVATE HELPER FUNCTIONS ***********************************************************************/

/**
 * @brief Appends an unsigned 64-bit integer into a destination buffer with custom base and casing.
 */
static SSize_t prv_append_uint(char *buf, SSize_t offset, SSize_t max_len, uint64_t uval, uint32_t base, int uppercase)
{
    char tmp[32];
    int idx = 0;

    /* Extract digits in reverse order */
    do
    {
        uint32_t rem = (uint32_t)(uval % base);
        char base_char = uppercase ? 'A' : 'a';
        tmp[idx++] = (char)((rem < 10U) ? ('0' + rem) : (base_char + (rem - 10U)));
        uval /= base;
    } while (uval > 0ULL && idx < (int)sizeof(tmp));

    /* Flush reversed digits into destination buffer */
    while (idx > 0 && offset < max_len)
    {
        buf[offset++] = tmp[--idx];
    }

    /* Return current write position */
    return offset;
}

/**
 * @brief Appends a signed 64-bit integer into a destination buffer.
 */
static SSize_t prv_append_int(char *buf, SSize_t offset, SSize_t max_len, int64_t val)
{
    uint64_t uval;

    /* Handle sign for negative decimal value */
    if (val < 0LL)
    {
        /* Check buffer boundary before appending negative sign */
        if (offset >= max_len)
        {
            return offset;
        }
        buf[offset++] = '-';
        uval = (uint64_t)(-val);
    }
    else
    {
        uval = (uint64_t)val;
    }

    /* Format unsigned magnitude */
    return prv_append_uint(buf, offset, max_len, uval, 10U, 0);
}

/**
 * @brief Appends a double-precision floating-point number into a destination buffer.
 */
static SSize_t prv_append_float(char *buf, SSize_t offset, SSize_t max_len, double val, uint32_t precision)
{
    /* Handle negative floating-point value */
    if (val < 0.0)
    {
        /* Check buffer boundary before appending negative sign */
        if (offset >= max_len)
        {
            return offset;
        }
        buf[offset++] = '-';
        val = -val;
    }

    /* Extract integral part */
    uint64_t int_part = (uint64_t)val;
    double frac_part = val - (double)int_part;

    /* Round fraction based on precision */
    double rounder = 0.5;
    for (uint32_t i = 0U; i < precision; i++)
    {
        rounder /= 10.0;
    }
    frac_part += rounder;

    /* Handle potential overflow to integer part */
    if (frac_part >= 1.0)
    {
        int_part++;
        frac_part -= 1.0;
    }

    /* Append integer portion */
    offset = prv_append_uint(buf, offset, max_len, int_part, 10U, 0);

    /* Append decimal fraction portion */
    if (precision > 0U && offset < max_len)
    {
        buf[offset++] = '.';

        for (uint32_t i = 0U; i < precision && offset < max_len; i++)
        {
            frac_part *= 10.0;
            uint32_t digit = (uint32_t)frac_part;
            buf[offset++] = (char)('0' + digit);
            frac_part -= (double)digit;
        }
    }

    /* Return current write position */
    return offset;
}

/* PUBLIC API IMPLEMENTATION **********************************************************************/

/**
 * @brief Reset Dst string.
 */
void Str_Clear(String_t *Dst)
{
    /* Validate destination pointers */
    if (Dst == NULL || Dst->buff == NULL)
    {
        return;
    }

    /* Clear string state */
    Dst->buff[0] = '\0';
    Dst->size = 0;
}

/**
 * @brief Copy Src into Dst, output truncated at Dst->max_size - 2.
 */
void Str_Copy(String_t *Dst, SSize_t Offset, String_t Src)
{
    /* Validate input pointers */
    if (Dst == NULL || Dst->buff == NULL || Src.buff == NULL)
    {
        return;
    }

    /* Check offset boundaries */
    if (Offset < 0 || Offset >= Dst->max_size)
    {
        return;
    }

    SSize_t max_allowed = Dst->max_size - 2;
    SSize_t i = 0;

    /* Copy characters up to limit */
    while ((i < Src.size) && ((Offset + i) < max_allowed))
    {
        Dst->buff[Offset + i] = Src.buff[i];
        i++;
    }

    /* Terminate string and save size */
    Dst->buff[Offset + i] = '\0';
    Dst->size = (int16_t)(Offset + i);
}

/**
 * @brief Concatenate N strings, output truncated at Dst->max_size - 2.
 */
void Str_Concat(String_t *Dst, SSize_t Offset, SSize_t N, String_t Src0, ...)
{
    /* Validate inputs */
    if (Dst == NULL || Dst->buff == NULL || N <= 0)
    {
        return;
    }

    /* Check offset boundary */
    if (Offset < 0 || Offset >= Dst->max_size)
    {
        return;
    }

    va_list args;
    va_start(args, Src0);

    SSize_t current_offset = Offset;
    SSize_t max_allowed = Dst->max_size - 2;
    String_t current_src = Src0;

    /* Traverse variable source strings */
    for (SSize_t idx = 0; idx < N; idx++)
    {
        /* Fetch subsequent string arguments */
        if (idx > 0)
        {
            current_src = va_arg(args, String_t);
        }

        /* Concatenate valid source */
        if (current_src.buff != NULL)
        {
            SSize_t i = 0;
            while ((i < current_src.size) && (current_offset < max_allowed))
            {
                Dst->buff[current_offset] = current_src.buff[i];
                current_offset++;
                i++;
            }
        }
    }

    va_end(args);

    /* Terminate string and record length */
    Dst->buff[current_offset] = '\0';
    Dst->size = (int16_t)current_offset;
}

/**
 * @brief Find the first position of Src inside Dst.
 */
SSize_t Str_Find(String_t *Dst, SSize_t Offset, String_t Src)
{
    /* Validate input pointers */
    if (Dst == NULL || Dst->buff == NULL || Src.buff == NULL)
    {
        return -1;
    }

    /* Validate search boundaries */
    if (Offset < 0 || Offset > Dst->size || Src.size <= 0)
    {
        return -1;
    }

    /* Search character matching pattern */
    for (SSize_t i = Offset; i <= Dst->size - Src.size; i++)
    {
        int match = 1;
        for (SSize_t j = 0; j < Src.size; j++)
        {
            /* Check character match */
            if (Dst->buff[i + j] != Src.buff[j])
            {
                match = 0;
                break;
            }
        }

        /* Return first matching index */
        if (match)
        {
            return i;
        }
    }

    /* Return failure indicator */
    return -1;
}

/**
 * @brief Repeat fill Src into Dst, output truncated at Dst->max_size - 2.
 */
void Str_Fill(String_t *Dst, SSize_t Offset, String_t Src)
{
    /* Validate inputs */
    if (Dst == NULL || Dst->buff == NULL || Src.buff == NULL || Src.size <= 0)
    {
        return;
    }

    /* Check offset range */
    if (Offset < 0 || Offset >= Dst->max_size)
    {
        return;
    }

    SSize_t max_allowed = Dst->max_size - 2;
    SSize_t current_offset = Offset;

    /* Repeatedly copy source buffer */
    while (current_offset < max_allowed)
    {
        SSize_t i = 0;
        while ((i < Src.size) && (current_offset < max_allowed))
        {
            Dst->buff[current_offset] = Src.buff[i];
            current_offset++;
            i++;
        }
    }

    /* Finalize null-terminated string */
    Dst->buff[current_offset] = '\0';
    Dst->size = (int16_t)current_offset;
}

/**
 * @brief Formats and prints text into Dst without using stdio.
 */
void Str_Printf(String_t *Dst, const char *fmt, ...)
{
    /* Validate input pointers */
    if (Dst == NULL || Dst->buff == NULL || fmt == NULL)
    {
        return;
    }

    va_list args;
    va_start(args, fmt);

    SSize_t max_allowed = Dst->max_size - 1;
    SSize_t offset = 0;

    /* Iterate through format string */
    while (*fmt != '\0' && offset < max_allowed)
    {
        /* Check format specifier marker */
        if (*fmt == '%')
        {
            fmt++;
            /* Check unexpected string termination */
            if (*fmt == '\0')
            {
                break;
            }

            int is_long = 0;

            /* Parse optional length modifier 'l' */
            if (*fmt == 'l')
            {
                is_long = 1;
                fmt++;
                /* Check premature format termination after length modifier */
                if (*fmt == '\0')
                {
                    break;
                }
            }

            /* Handle specifiers */
            switch (*fmt)
            {
                case 's':
                {
                    const char *s = va_arg(args, const char*);
                    /* Fallback to null string representation */
                    if (s == NULL)
                    {
                        s = "(null)";
                    }
                    while (*s != '\0' && offset < max_allowed)
                    {
                        Dst->buff[offset++] = *s++;
                    }
                    break;
                }

                case 'c':
                {
                    char c = (char)va_arg(args, int);
                    Dst->buff[offset++] = c;
                    break;
                }

                case 'd':
                case 'i':
                {
                    int64_t val = is_long ? (int64_t)va_arg(args, long) : (int64_t)va_arg(args, int);
                    offset = prv_append_int(Dst->buff, offset, max_allowed, val);
                    break;
                }

                case 'u':
                {
                    uint64_t uval = is_long ? (uint64_t)va_arg(args, unsigned long) : (uint64_t)va_arg(args, unsigned int);
                    offset = prv_append_uint(Dst->buff, offset, max_allowed, uval, 10U, 0);
                    break;
                }

                case 'x':
                {
                    uint64_t uval = is_long ? (uint64_t)va_arg(args, unsigned long) : (uint64_t)va_arg(args, unsigned int);
                    offset = prv_append_uint(Dst->buff, offset, max_allowed, uval, 16U, 0);
                    break;
                }

                case 'X':
                {
                    uint64_t uval = is_long ? (uint64_t)va_arg(args, unsigned long) : (uint64_t)va_arg(args, unsigned int);
                    offset = prv_append_uint(Dst->buff, offset, max_allowed, uval, 16U, 1);
                    break;
                }

                case 'p':
                {
                    uintptr_t ptr = (uintptr_t)va_arg(args, void*);
                    /* Append hex pointer prefix */
                    if (offset < max_allowed)
                    {
                        Dst->buff[offset++] = '0';
                    }
                    if (offset < max_allowed)
                    {
                        Dst->buff[offset++] = 'x';
                    }
                    offset = prv_append_uint(Dst->buff, offset, max_allowed, (uint64_t)ptr, 16U, 0);
                    break;
                }

                case 'f':
                {
                    double dval = va_arg(args, double);
                    offset = prv_append_float(Dst->buff, offset, max_allowed, dval, FLOAT_PRECISION_DEFAULT);
                    break;
                }

                case '%':
                {
                    Dst->buff[offset++] = '%';
                    break;
                }

                default:
                {
                    /* Output unrecognized character directly */
                    Dst->buff[offset++] = '%';
                    if (is_long && offset < max_allowed)
                    {
                        Dst->buff[offset++] = 'l';
                    }
                    if (offset < max_allowed)
                    {
                        Dst->buff[offset++] = *fmt;
                    }
                    break;
                }
            }
        }
        else
        {
            Dst->buff[offset++] = *fmt;
        }

        fmt++;
    }

    va_end(args);

    /* Terminate destination buffer */
    Dst->buff[offset] = '\0';
    Dst->size = (int16_t)offset;
}