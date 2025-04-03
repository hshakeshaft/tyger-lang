#ifndef TYGER_CONTAINERS_H_
#define TYGER_CONTAINERS_H_
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

/// Default number of elements to initially allocate dynamic array with
#define DA_DEFAULT_CAPACITY 64

/// Initialises a dynamic array.
///
/// @note These macros describe an interface for interacting with dynamic arrays.
/// @note Dynamic Arrays are assumed to have the following structure:
/// struct { size_t capacity; size_t len; T *elements; } where `T` is a declared type
/// of the underlying buffer.
///
/// @param TYPE the data type of the underlying array
/// @param DA pointer to the dynamic array to initialise the data of
///
/// @example da_init(int, &int_da);
#define da_init(TYPE, DA)                                                                                  \
    do {                                                                                                   \
        (DA)->capacity = DA_DEFAULT_CAPACITY;                                                              \
        (DA)->len = 0;                                                                                     \
        (DA)->elements = malloc(sizeof(TYPE) * (DA)->capacity);                                            \
        assert((DA)->elements && "Failed to allocate space for dynamic array elements in initialisation"); \
    } while (0)

/// Frees a specified dynamic array, sets the elements poitner to NULL, and zeros
/// the capacity and length members.
///
/// @param DA Pointer to the dynamic array to free
#define da_free(DA)                \
    do {                           \
        if ((DA)->elements) {      \
            free((DA)->elements);  \
            (DA)->elements = NULL; \
        }                          \
        (DA)->capacity = 0;        \
        (DA)->len = 0;             \
    } while (0)

/// Append an element to a dynamic array
///
/// @param TYPE The type of the elements within the underlying buffer
/// @param DA Pointer to the dynamic array to append elements to
/// @param VAL Pointer to the value to append into the dynamic array
#define da_append(TYPE, DA, VAL)                                      \
    do {                                                              \
        if ((DA)->len + 1 > (DA)->capacity) {                         \
            size_t new_capacity = (DA)->capacity * 2;                 \
            TYPE *new_buffer = realloc((DA)->elements, new_capacity); \
            if (new_buffer != (DA)->elements) {                       \
                (DA)->elements = new_buffer;                          \
            }                                                         \
            (DA)->capacity = new_capacity;                            \
        }                                                             \
        memcpy(&((DA)->elements[(DA)->len]), VAL, sizeof(TYPE));      \
        (DA)->len += 1;                                               \
    } while (0)

// resize dynamic array down to the number of elements actually neeeded
// #define da_resize(DA)

#endif // TYGER_CONTAINERS_H_
