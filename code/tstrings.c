#include <string.h>
#include "tstrings.h"

size_t string_nlen(const char *str, size_t max_len)
{
    size_t len = 0;
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

inline bool is_whitespace(const char c)
{
    return c == '\n' || c == '\r' || c == '\t' || c == ' ';
}

inline bool is_numeric(const char c)
{
    return ('0' <= c && c <= '9');
}

inline bool is_alpha(const char c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

inline bool is_alphanumeric(const char c)
{
    return is_alpha(c) || is_numeric(c);
}

inline bool is_punctuation(const char c)
{
    bool result = false;
    const char punctuation[] = ";:,.";
    for (size_t i = 0; i < (sizeof(punctuation) / sizeof(punctuation[0])); ++i)
    {
        if (c == punctuation[i])
        {
            result = true;
            break;
        }
    }
    return result;
}

inline bool is_end_of_input(const char c)
{
    return c == '\0';
}
