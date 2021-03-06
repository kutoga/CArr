#ifndef CARR_H
#define CARR_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

/* Internal base library */

typedef uint32_t _arr_d_marker_type;
typedef uint16_t _arr_c_marker_type;

#define _arr_selector(arr, exp_d, exp_c)                            \
_Generic(                                                           \
    (arr)->_marker[0],                                              \
    _arr_d_marker_type: (exp_d),                                    \
    _arr_c_marker_type: (exp_c)                                     \
)

#define _arr_inline                                                 \
inline __attribute__((always_inline))

#define _arr_likely(x)                                              \
__builtin_expect(!!(x), 1)
#define _arr_unlikely(x)                                            \
__builtin_expect(!!(x), 0)

#ifdef __cplusplus
#define _arr_auto                                                   auto
#else
#define _arr_auto                                                   __auto_type
#endif

static inline void _arr_fatal(const char *message)
{
    fprintf(stderr, "Fatal error: %s\n", message);
    exit(1);
}

static inline unsigned int _arr_round_up_pow2(unsigned int n)
{
    if (n < 2) {
        return n;
    }

    const unsigned int res = 1u << (sizeof(n) * CHAR_BIT - __builtin_clz(n - 1));
    return res < n ? 0 : res;
}

/*
 * Type definitions
 * A zero-sized type marker is used in order to distinguish at compile-time
 * between t_arr_d(T) and t_arr_c(T, N). Note that the type-marker must not
 * be the first element, otherwise the array can no longer be initialized
 * with "... = {0};"
 */

#define t_arr_d(type)                                               \
struct {                                                            \
    size_t count;                                                   \
    _arr_d_marker_type _marker[0];                                  \
    size_t capacity;                                                \
    type *elements;                                                 \
}

#define t_arr_c(type, capacity)                                     \
struct {                                                            \
    size_t count;                                                   \
    _arr_c_marker_type _marker[0];                                  \
    type elements[capacity];                                        \
}

/*
 * void arr_destroy(t_arr *arr)
 */

static _arr_inline void _arr_d_destroy(void *arr_obj)
{
    t_arr_d(void) *arr = arr_obj;
    free(arr->elements);
    arr->elements = NULL;
    arr->capacity = 0;
}

#define arr_destroy(arr)                                            \
do {                                                                \
    _arr_auto _arr = (arr);                                         \
    (void)_arr_selector(_arr, _arr_d_destroy(arr), 0);              \
    _arr->count = 0;                                                \
} while (0)

/*
 * size_t arr_count(t_arr *arr)
 */

#define arr_count(arr)                                              \
(arr)->count

/*
 * size_t arr_capacity(t_arr *arr)
 * size_t arr_c_capacity(t_arr_c(T, N) *arr) <-- compile time constant
 */

static _arr_inline size_t _arr_d_capacity(void *arr_obj)
{
    t_arr_d(void) *arr = arr_obj;
    return arr->capacity;
}

#define arr_c_capacity(arr)                                         \
(sizeof((arr)->elements) / sizeof((arr)->elements[0]))

#define arr_capacity(arr)                                           \
({                                                                  \
    const _arr_auto _arr = (arr);                                   \
    _arr_selector(                                                  \
        _arr,                                                       \
        _arr_d_capacity(_arr),                                      \
        arr_c_capacity(_arr)                                        \
    );                                                              \
})

/*
 * void arr_clear(t_arr *arr)
 */

#define arr_clear(arr)                                              \
do {                                                                \
    (arr)->count = 0;                                               \
} while (0)

/*
 * void arr_ensure_capacity(t_arr *arr)
 */

static inline void _arr_d_ensure_capacity(void *arr_obj, size_t capacity, size_t element_size)
{
    t_arr_d(void) *arr = arr_obj;
    if (_arr_unlikely(arr->capacity < capacity)) {
        const size_t min_capacity = 8;
        if (capacity < min_capacity) {
            arr->capacity = min_capacity;
        }
        else {
            arr->capacity = _arr_round_up_pow2(capacity);
        }
        arr->elements = realloc(arr->elements, arr->capacity * element_size);
    }
}

static inline void _arr_c_ensure_capacity(size_t actual_capacity, size_t capacity)
{
    if (_arr_unlikely(actual_capacity < capacity)) {
        _arr_fatal("Cannot increase capacity of arr_c(...)!");
    }
}

#define arr_ensure_capacity(arr, capacity)                          \
do {                                                                \
    const _arr_auto _arr = (arr);                                   \
    const size_t _capacity = (capacity);                            \
    _arr_selector(                                                  \
        _arr,                                                       \
        _arr_d_ensure_capacity(_arr, _capacity),                    \
        _arr_c_ensure_capacity(arr_c_capacity(_arr), _capacity,     \
                               sizeof(_arr->elements[0]))           \
    )                                                               \
} while (0)

/*
 * void arr_add_ptr(t_arr *arr, const T *element)
 */

static _arr_inline bool _arr_d_add_ptr(void *arr_obj, const void *element, size_t element_size)
{
    t_arr_d(char) *arr = arr_obj;
    _arr_d_ensure_capacity(arr, arr->count + 1, element_size);
    memcpy(&arr->elements[element_size * arr->count], element, element_size);
    ++arr->count;
    return true;
}

static _arr_inline size_t _arr_c_add_ptr(void *arr_obj, size_t actual_capacity, const void *element, size_t element_size)
{
    t_arr_c(char, 1) *arr = arr_obj;
    const bool _ok = actual_capacity > arr->count;
    if (_arr_likely(_ok)) {
        memcpy(&arr->elements[element_size * arr->count], element, element_size);
        ++arr->count;
    }
    return _ok;
}

#define arr_add_ptr(arr, element)                                   \
({                                                                  \
    _arr_auto _arr = (arr);                                         \
    const typeof(_arr->elements[0]) *_element =                     \
        /*(typeof(&_arr->elements[0]))*/(element);                  \
    _arr_selector(                                                  \
        _arr,                                                       \
        _arr_d_add_ptr(_arr, _element, sizeof(*_element)),          \
        _arr_c_add_ptr(_arr, arr_c_capacity(_arr),                  \
                    _element, sizeof(*_element))                    \
    );                                                              \
})

/*
 * void arr_add(t_arr *arr, T element)
 */

#define arr_add(arr, element)                                       \
({                                                                  \
    const _arr_auto _arr_p = (arr);                                 \
    const typeof(_arr_p->elements[0]) _element_p = (element);       \
    arr_add_ptr(_arr_p, &_element_p);                               \
})

/*
 * const T *arr_ptr_at(const t_arr *arr, size_t index)
 *       T *arr_ptr_at(t_arr *arr,       size_t index)
 */

static inline void _arr_check_bounds(size_t count, size_t index)
{
    if (_arr_unlikely(index) >= count) {
        _arr_fatal("Index out of range!");
    }
}

#define arr_ptr_at(arr, index)                                      \
({                                                                  \
    _arr_auto _arr = (arr);                                         \
    const size_t _index = (index);                                  \
    _arr_check_bounds(_arr->count, _index);                         \
    &_arr->elements[_index];                                        \
})

/*
 * T arr_at(const t_arr *arr, size_t index)
 */

#define arr_at(arr, index)                                          \
(*arr_ptr_at((arr), (index)))

/*
 * const T *arr_ptr_at_unsafe(const t_arr *arr, size_t index)
 *       T *arr_ptr_at_unsafe(t_arr *arr,       size_t index)
 */

#define arr_ptr_at_unsafe(arr, index)                               \
(&(arr)->elements[index])

/*
 * T arr_at_unsafe(const t_arr *arr, size_t index)
 */

#define arr_at_unsafe(arr, index)                                   \
((arr)->elements[index])

/*
 * arr_foreach_ptr (const t_arr *arr, iter)
 *   iter has these fields:
 *   - size_t index
 *   - T *value
 */

#define arr_foreach_ptr(arr, iter)                                  \
for (_arr_auto iter = ({ struct {                                   \
        size_t index;                                               \
        typeof((arr)->elements[0]) *value;                          \
    } _res = {.index = 0}; _res; }); ({                             \
    const _arr_auto _arr = (arr);                                   \
    const bool _ok = iter.index < _arr->count;                      \
    if (_ok) {                                                      \
        iter.value = &_arr->elements[iter.index];                   \
    }                                                               \
    _ok;                                                            \
}); ++iter.index)

/*
 * arr_foreach (const t_arr *arr, iter)
 *   iter has these fields:
 *   - size_t index
 *   - T value
 */

#define arr_foreach(arr, iter)                                      \
for (_arr_auto iter = ({ struct {                                   \
        size_t index;                                               \
        typeof((arr)->elements[0]) value;                           \
    } _res = {.index = 0}; _res; }); ({                             \
    const _arr_auto _arr = (arr);                                   \
    const bool _ok = iter.index < _arr->count;                      \
    if (_ok) {                                                      \
        iter.value = _arr->elements[iter.index];                    \
    }                                                               \
    _ok;                                                            \
}); ++iter.index)

#endif /* CARR_H */
