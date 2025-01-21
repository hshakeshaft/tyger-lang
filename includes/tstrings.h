#ifndef TYGER_STRING_H_
#define TYGER_STRING_H_
#include <stdbool.h>
#include <stddef.h>

typedef struct
{
    char *str;
    size_t length;
} String_View;

#define sv_fmt "%.*s"
#define sv_args(S) (int) (S).length, (S).str

#if defined(__cplusplus)
extern "C" {
#endif

size_t string_nlen(const char *str, size_t max_len);

String_View string_view_from_cstr(const char *str, size_t offset);
String_View string_view_from_cstr_offset(const char *str, size_t offset, size_t length);

bool string_view_eq(String_View s1, String_View s2);
bool string_view_eq_cstr(String_View sv, const char *s);

// NOTE(HS): all assume ASCII encoding
bool is_whitespace(const char c);
bool is_numeric(const char c);
bool is_alpha(const char c);
bool is_alphanumeric(const char c);
bool is_punctuation(const char c);
bool is_end_of_input(const char c);

#if defined(__cplusplus)
}
#endif

#endif // TYGER_STRING_H_
