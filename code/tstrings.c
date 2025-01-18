#include <string.h>
#include "tstrings.h"

size_t string_nlen(const char *str, size_t max_len)
{
    size_t len = 1;
    size_t i = 0;

    for (;;)
    {
        if (str[i++] == '\0' || len == max_len)
        {
            break;
        }
        len += 1;
    }

    return len;
}

inline String_View string_view_from_cstr(const char *str, size_t length)
{
    return (String_View){ (char*) str, length };
}

inline String_View string_view_from_cstr_offset(
    const char *str, 
    size_t offset, 
    size_t length
)
{
    return string_view_from_cstr((char*) &str[offset], length);
}

bool string_view_eq(String_View s1, String_View s2)
{
    if (s1.length != s2.length)
    {
        return false;
    }

    for (size_t i = 0; i < s1.length; ++i)
    {
        if (s1.str[i] != s2.str[i])
        {
            return false;
        }
    }

    return true;
}

bool string_view_eq_cstr(String_View sv, const char *s)
{
    if (!s || s == NULL || *s == '\0')
    {
        return false;
    }

    size_t s_len = string_nlen(s, 100);
    if (sv.length != s_len)
    {
        return false;
    }

    return true;
}
